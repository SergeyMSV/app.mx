using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;
using ExtensionMethods;

namespace DallasRW
{
    public partial class FormMain : Form
    {
        Process? m_DallasProcess;
        UdpClient? m_DallasUDPClient;
        bool m_DallasConnected = false;
        const string m_FileKeysPath = "keys.json";
        readonly Mutex m_DallasProcessMtx = new Mutex();
        readonly string m_Text;
        readonly string m_TextKeyIDEmpty;

        public FormMain()
        {
            InitializeComponent();

            m_Text = Text;
            m_TextKeyIDEmpty = labelRead.Text;

            Restart();
            CheckDallasProcess();

            LoadTreeView();
            buttonAdd.Enabled = true;
        }

        void LoadTreeView()
        {
            try
            {
                StreamReader sr = new StreamReader(m_FileKeysPath);
                string DataJSON = sr.ReadToEnd();
                sr.Close();

                treeViewKeys.BeginUpdate();
                treeViewKeys.Nodes.Clear();

                JsonObject JObj = JsonNode.Parse(DataJSON)?.AsObject()!;
                foreach (var i in JObj)
                {
                    TreeNode NodeNew = treeViewKeys.Nodes.Add(i.Key);
                    NodeNew.Tag = i;

                    JsonObject JObjSub = i.Value?.AsObject()!;
                    foreach (var j in JObjSub)
                    {
                        string LineValue = j.Value?.ToString() + "     " + j.Key;
                        TreeNode SubNodeNew = NodeNew.Nodes.Add(j.Key, LineValue);
                        SubNodeNew.Tag = j;
                    }
                }

                treeViewKeys.Sort();
                treeViewKeys.EndUpdate();
                treeViewKeys.ExpandAll();
                // [TBD] select needed item
                buttonEdit.Enabled = false; // [TBD] remove it
                buttonDelete.Enabled = false; // [TBD] remove it
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        void ResetKeyID()
        {
            labelRead.Text = m_TextKeyIDEmpty;
            buttonSave.Enabled = false;
            buttonWrite.Enabled = false;
        }

        void DallasDecoder(IPEndPoint ep, string rsp)
        {
            try
            {
                Cursor = Cursors.Default;

                Log.WriteTrace("Receive from " + ep.ToString() + "\n" + rsp);

                JsonNode Node = JsonNode.Parse(rsp)!;
                JsonNode NodeCmd = Node!["cmd"]!;
                JsonNode NodeStatus = Node!["rsp"]!;

                string Status = NodeStatus == null ? "" : NodeStatus.ToString();

                if (Status != "ok")
                    MessageBox.Show("Response: " + Status + ".");

                switch (NodeCmd.ToString())
                {
                    case "read_key":
                        labelRead.Text = m_TextKeyIDEmpty;

                        JsonNode NodeKeyID = Node["id"]!;
                        if (NodeKeyID != null)
                            labelRead.Text = NodeKeyID.ToString();
                        if (Status == "ok")
                        {
                            buttonSave.Enabled = true;
                            buttonWrite.Enabled = true;
                        }
                        else
                        {
                            ResetKeyID();
                        }
                        break;
                    case "write_key":
                        if (Status == "ok")
                            MessageBox.Show("The key ID has been written successfully.");
                        break;
                    case "version":
                        if (Status == "ok")
                        {
                            string Ver = Node["version"]?.ToString()!;
                            Text = m_Text + ": " + Ver + " (" + ep.ToString() + ")";
                            m_DallasConnected = true;
                            buttonRead.Enabled = true;
                            buttonRead.Focus();
                        }
                        break;
                }
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        void Send(string msg)
        {
            try
            {
                string IPAddrStr = Properties.Settings.Default.Localhost ? "127.0.0.1" : Properties.Settings.Default.IPAddressRemote;
                IPEndPoint Ep = new IPEndPoint(IPAddress.Parse(IPAddrStr), Properties.Settings.Default.UDPPortRemote);

                Byte[] Req = Encoding.ASCII.GetBytes(msg);
                m_DallasUDPClient?.Send(Req, Req.Length, Ep);
            }
            catch { }
        }

        #region Restart

        void SetStateNotConnected()
        {
            m_DallasConnected = false;

            Text = m_Text + ": not connected";

            buttonRead.Enabled = false;
            buttonWrite.Enabled = false;

            Cursor = Cursors.Default;
        }

        void Restart()
        {
            SetStateNotConnected();

            Cursor = Cursors.WaitCursor;
            RestartDallasProcess();
            RestartDallasUDP();
            ReqDallasVersion();
            Cursor = Cursors.Default;
        }

        bool RestartDallasUDP()
        {
            try
            {
                if (m_DallasUDPClient != null)
                    m_DallasUDPClient.Close();

                m_DallasUDPClient = new UdpClient(Properties.Settings.Default.UDPPortLocal);
                m_DallasUDPClient.EnableBroadcast = false;
                m_DallasUDPClient.DontFragment = true;
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
                return false;
            }

            Task.Run(async () => // [*] it doesn't increase amount of threads for some unknown reason
            {
                try
                {
                    while (true)
                    {
                        UdpReceiveResult RecvResult = await m_DallasUDPClient.ReceiveAsync();
                        string RecvStr = Encoding.Default.GetString(RecvResult.Buffer);
                        Invoke(new Action(() => DallasDecoder(RecvResult.RemoteEndPoint, RecvStr))); // [TBD] is it OK ?
                    }
                }
                catch (Exception ex)
                {
                    Log.WriteError(ex.Message);
                }
            });

            return true;
        }

        string GetDallasProcessExitMsg()
        {
            string Msg = "";
            if (m_DallasProcess != null && m_DallasProcess.HasExited)
            {
                Msg = "Process " + m_DallasProcess.StartInfo.FileName + " hasn't been started. Exit code: " + m_DallasProcess.ExitCode.ToString() + ".";
                if (m_DallasProcess.ExitCode == 74) // EX_IOERR = 74, input/output error
                    Msg += "\nPerhaps Serial Port failed to open or connection has been broken.";
            }
            return Msg;
        }

        bool RestartDallasProcess()
        {
            m_DallasProcessMtx.WaitOne();

            try
            {
                if (m_DallasProcess != null && !m_DallasProcess.HasExited)
                    m_DallasProcess.Kill();

                m_DallasProcess = null;

                if (Properties.Settings.Default.Localhost)
                {
                    Thread.Sleep(500); // Waiting for killing the process.

                    ProcessStartInfo StartInfo = new ProcessStartInfo();
                    StartInfo.CreateNoWindow = true;
                    StartInfo.FileName = "mxdallas.exe";
                    StartInfo.UseShellExecute = false;
                    StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
                    StartInfo.Arguments = Properties.Settings.Default.UDPPortRemote.ToString() + " COM" + Properties.Settings.Default.COMPortNumber.ToString();

                    m_DallasProcess = Process.Start(StartInfo)!;
                    Thread.Sleep(100); // in order for HasExited to be defined
                    if (m_DallasProcess!.HasExited)
                    {
                        string Msg = GetDallasProcessExitMsg();
                        m_DallasProcess = null;
                        MessageBox.Show(Msg);
                        throw new Exception(Msg);
                    }
                }
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
                return false;
            }
            finally
            {
                m_DallasProcessMtx.ReleaseMutex();
            }
            return true;
        }

        void ReqDallasVersion()
        {
            Task.Run(() =>
            {
                Thread.Sleep(100); // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."

                Send("{\"cmd\":\"version\"}");
            });
        }

        void CheckDallasProcess()
        {
            Task.Run(() =>
            {
                bool HasExitedPrev = m_DallasProcess != null && m_DallasProcess.HasExited;

                while (true)
                {
                    Thread.Sleep(1000);

                    if (m_DallasProcess == null)
                        continue;

                    m_DallasProcessMtx.WaitOne();
                    if (m_DallasProcess == null)
                    {
                        m_DallasProcessMtx.ReleaseMutex();
                        continue;
                    }

                    if (m_DallasProcess.HasExited && HasExitedPrev != m_DallasProcess.HasExited)
                    {
                        string Msg = GetDallasProcessExitMsg();
                        MessageBox.Show(Msg);
                        Invoke(new Action(() =>
                        {
                            SetStateNotConnected();
                        }));
                    }
                    HasExitedPrev = m_DallasProcess.HasExited;
                    m_DallasProcessMtx.ReleaseMutex();
                }
            });
        }

        #endregion

        #region Button_Click & other events

        void buttonRead_Click(object sender, EventArgs e)
        {
            Cursor = Cursors.WaitCursor;
            Send("{\"cmd\":\"read_key\"}");
        }

        void buttonWrite_Click(object sender, EventArgs e)
        {
            if (!checkBoxWrite.Checked)
            {
                string MsgText = checkBoxWrite.Text.Replace("&", string.Empty);
                MessageBox.Show("Set \"" + MsgText + "\" first");
                return;
            }

            Cursor = Cursors.WaitCursor;
            Send("{\"cmd\":\"write_key\",\"id\":\"" + labelRead.Text + "\"}");
        }

        void buttonSave_Click(object sender, EventArgs e)
        {
            SaveKeyID();
        }

        void buttonAdd_Click(object sender, EventArgs e)
        {
            AddKeyID();
        }

        void buttonEdit_Click(object sender, EventArgs e)
        {
            EditKeyID();
        }

        void buttonDelete_Click(object sender, EventArgs e)
        {
            RemoveKeyID();
        }

        void buttonSettings_Click(object sender, EventArgs e)
        {
            FormSettings FormSettings = new FormSettings();
            FormSettings.ShowDialog();
            if (FormSettings.IsChanged)
                Restart();
        }

        void buttonReconnect_Click(object sender, EventArgs e)
        {
            Restart();
        }

        void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_DallasProcess?.Kill();
            m_DallasUDPClient?.Close();
        }

        #endregion

        void SetKeyID()
        {
            try
            {
                TreeNode TreeNodeSelected = treeViewKeys.SelectedNode;
                if (TreeNodeSelected == null ||
                    TreeNodeSelected.Parent == null ||
                    TreeNodeSelected.Tag == null)
                    return;
                var JNode = (KeyValuePair<string, JsonNode?>)TreeNodeSelected.Tag;
                labelRead.Text = JNode.Value?.ToString();
                buttonWrite.Enabled = m_DallasConnected;
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        void FormKeyIDShow(string filePath, TreeNode? nodeSelected, string keyID)
        {
            try
            {
                string KeyGroup = "";
                string KeyLabel = "";
                if (nodeSelected != null)
                {
                    if (nodeSelected.Parent?.FullPath != null)
                        KeyGroup = nodeSelected.Parent.FullPath;

                    if (nodeSelected.Tag != null)
                    {
                        var JNode = (KeyValuePair<string, JsonNode?>)nodeSelected.Tag;
                        if (nodeSelected.Parent == null)
                        {
                            KeyGroup = JNode.Key;
                        }
                        else
                        {
                            KeyLabel = JNode.Key;
                            if (JNode.Value != null)
                                keyID = JNode.Value.ToString();
                        }
                    }
                }
                FormKeyID FormKeyID = new FormKeyID(m_FileKeysPath, KeyGroup, KeyLabel, keyID);
                FormKeyID.ShowDialog();
                if (FormKeyID.IsChanged)
                    LoadTreeView();
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        void SaveKeyID()
        {
            if (!labelRead.Text.IsKeyID())
                return;
            FormKeyIDShow(m_FileKeysPath, null, labelRead.Text);
        }

        void AddKeyID()
        {
            FormKeyIDShow(m_FileKeysPath, null, "");
        }

        void EditKeyID()
        {
            TreeNode TreeNodeSelected = treeViewKeys.SelectedNode;
            if (TreeNodeSelected?.Parent == null)
                return;
            FormKeyIDShow(m_FileKeysPath, treeViewKeys.SelectedNode, "");
        }

        void RemoveKeyID()
        {
           try
            {
                TreeNode TreeNodeSelected = treeViewKeys.SelectedNode;
                if (TreeNodeSelected == null ||
                    TreeNodeSelected.Nodes.Count != 0 ||
                    TreeNodeSelected.Tag == null)
                    return;
                var JNode = (KeyValuePair<string, JsonNode?>)TreeNodeSelected.Tag;

                string Msg = "Are you sure you want to permanently delete this item?\n\n";
                Msg += "Group:  " + TreeNodeSelected.Parent.FullPath + "\n";
                Msg += "Label:  " + JNode.Key + "\n";
                Msg += "Key ID: " + JNode.Value + "\n";

                if (MessageBox.Show(Msg, "Delete", MessageBoxButtons.YesNo) == DialogResult.No)
                    return;

                FileKeys.Remove(m_FileKeysPath, TreeNodeSelected.Parent.FullPath, JNode.Key);
                LoadTreeView();
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        void treeViewKeys_DoubleClick(object sender, EventArgs e)
        {
            SetKeyID();
        }

        void treeViewKeys_AfterSelect(object sender, TreeViewEventArgs e)
        {
            buttonEdit.Enabled = e.Node?.Parent != null;
            buttonDelete.Enabled = e.Node?.Parent != null;
        }

        void FormMain_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.Escape:
                    ResetKeyID();
                    break;
                case Keys.Enter:
                    SetKeyID();
                    break;
                case Keys.Delete:
                    RemoveKeyID();
                    break;
                case Keys.F2:
                    EditKeyID();
                    break;
                case Keys.C:
                    if (labelRead.Text != m_TextKeyIDEmpty)
                        Clipboard.SetText(labelRead.Text);
                    else
                        Clipboard.Clear();
                    break;
                case Keys.S:
                    if (e.Control == true)
                        SaveKeyID();
                    break;
            }
        }
    }
}