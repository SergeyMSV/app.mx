using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;

namespace TWR_Dallas
{
    public partial class FormMain : Form
    {
        UdpClient? m_UDPClient;
        bool m_RequestInProgress = false;
        readonly string m_Text;
        uint m_AutoGetValuePeriod = Properties.Settings.Default.AutoGetValuePeriod;
        int m_AutoGetValuePeriodCounter = 0;

        public FormMain()
        {
            InitializeComponent();

            m_Text = Text;

            Restart();
            AutoGetValue();
        }

        void ControlsEnabled(bool state)
        {
            Cursor = state ? Cursors.Default : Cursors.WaitCursor;
            buttonSearch.Enabled = state && !checkBoxAutoGetValue.Checked;
            buttonGetValue.Enabled = state && !checkBoxAutoGetValue.Checked;
        }

        void PacketDecoder(IPEndPoint ep, string rsp)
        {
            try
            {
                ControlsEnabled(true);
                m_RequestInProgress = false;

                Log.WriteTrace("Receive from " + ep.ToString() + "\n" + rsp);

                JsonNode Node = JsonNode.Parse(rsp)!;
                JsonNode NodeCmd = Node!["cmd"]!;
                JsonNode NodeResponse = Node!["rsp"]!;

                string Response = NodeResponse == null ? "" : NodeResponse.ToString();

                if (Response != "ok")
                    MessageBox.Show("Response: " + Response + ".");

                switch (NodeCmd.ToString())
                {
                    case "version":
                        {
                            string Ver = Node?["version"]?.ToString()! ?? "unknown";
                            Text = m_Text + ": " + Ver + " (" + ep.ToString() + ")";

                            ControlsEnabled(false);
                            Send("{\"ep\":\"dallas\",\"cmd\":\"open\"}");
                            break;
                        }
                    case "open":
                        {
                            if (Response != "ok")
                            {
                                ControlsEnabled(false);
                                break;
                            }
                            Send("{\"ep\":\"dallas\",\"cmd\":\"search\"}");
                            break;
                        }
                    case "search":
                        {
                            listViewBus.Items.Clear();

                            JsonNode NodeROMs = Node["roms"]!;
                            if (NodeROMs == null)
                            {
                                MessageBox.Show("There are no devices connected to the bus.");
                                break;
                            }

                            foreach (var rom in NodeROMs.AsArray())
                            {
                                JsonNode NodeFamilyCode = rom["family_code"]!;
                                JsonNode NodeID = rom["id"]!;
                                if (NodeFamilyCode == null || NodeID == null)
                                    continue;

                                ListViewItem item = new ListViewItem(NodeFamilyCode.ToString()); // DisplayIndex = 0
                                //item.Tag = DateTime.Now;
                                item.SubItems.Add(NodeID.ToString()); // DisplayIndex = 1
                                item.SubItems.Add("---"); // DisplayIndex = 2
                                listViewBus.Items.Add(item);
                            }
                            break;
                        }
                    case "thermo":
                        {
                            foreach (var i in listViewBus.Items)
                            {
                                ListViewItem Item = i as ListViewItem;
                                Item.SubItems[2].Text = "";
                            }

                            JsonNode NodeMeasurements = Node["measurements"]!;
                            if (NodeMeasurements == null)
                            {
                                MessageBox.Show("No measurements obtained.");
                                break;
                            }

                            foreach (var node in NodeMeasurements.AsArray())
                            {
                                JsonNode NodeID = node["id"]!;
                                JsonNode NodeTemperature = node["temperature"]!;
                                if (NodeID == null)
                                    continue;

                                foreach (var i in listViewBus.Items)
                                {
                                    ListViewItem Item = i as ListViewItem;
                                    if (Item.SubItems[1].Text == NodeID.ToString())
                                    {
                                        Item.SubItems[2].Text = NodeTemperature != null ? NodeTemperature.ToString() : "no measurement";
                                    }
                                }
                            }
                            break;
                        }
                }
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        void Send(string msg, bool forced = false)
        {
            try
            {
                if (m_RequestInProgress && !forced)
                    return;
                m_RequestInProgress = true;

                string IPAddrStr = Properties.Settings.Default.Localhost ? "127.0.0.1" : Properties.Settings.Default.IPAddressRemote;
                IPEndPoint Ep = new IPEndPoint(IPAddress.Parse(IPAddrStr), Properties.Settings.Default.UDPPortRemote);

                Byte[] Req = Encoding.ASCII.GetBytes(msg);
                m_UDPClient?.Send(Req, Req.Length, Ep);
            }
            catch { }
        }

        void GetValue()
        {
            ControlsEnabled(false);
            try
            {
                bool Empty = true;
                string CmdJSON = "{\"ep\":\"dallas\",\"cmd\":\"thermo\",\"ids\":[";
                foreach (var i in listViewBus.Items)
                {
                    ListViewItem Item = i as ListViewItem;
                    if (Item.SubItems[0].Text == "28") // DS18B20
                    {
                        if (!Empty)
                            CmdJSON += ",";
                        CmdJSON += "\"" + Item.SubItems[1].Text + "\"";
                        Empty = false;
                    }
                }
                CmdJSON += "]}";

                if (!Empty)
                    Send(CmdJSON);
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        void AutoGetValue()
        {
            Task.Run(() =>
            {
                while (true)
                {
                    try
                    {
                        // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."
                        Thread.Sleep(1000); // [TBD] it's possible to set sleeping period twice less.

                        if (!checkBoxAutoGetValue.Checked)
                            continue;
                        if (++m_AutoGetValuePeriodCounter < m_AutoGetValuePeriod)
                            continue;
                        m_AutoGetValuePeriodCounter = 0;

                        Invoke(new Action(() => GetValue()));
                    }
                    catch (Exception ex)
                    {
                        Log.WriteError(ex.Message);
                    }
                }
            });
        }

        #region Restart

        void SetStateNotConnected()
        {
            m_RequestInProgress = false;

            Text = m_Text + ": not connected";

            listViewBus.Items.Clear();
            ControlsEnabled(false);

            Cursor = Cursors.Default;
        }

        void Restart()
        {
            SetStateNotConnected();

            Cursor = Cursors.WaitCursor;
            if (!RestartUDP())
                return;
            ReqTWRVersion();
            m_AutoGetValuePeriodCounter = 0;
            Cursor = Cursors.Default;
        }

        bool RestartUDP()
        {
            try
            {
                if (m_UDPClient != null)
                    m_UDPClient.Close();

                m_UDPClient = new UdpClient(Properties.Settings.Default.UDPPortLocal);
                m_UDPClient.EnableBroadcast = false;
                m_UDPClient.DontFragment = true;
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
                        UdpReceiveResult RecvResult = await m_UDPClient.ReceiveAsync();
                        string RecvStr = Encoding.Default.GetString(RecvResult.Buffer);
                        Invoke(new Action(() => PacketDecoder(RecvResult.RemoteEndPoint, RecvStr))); // [TBD] is it OK ?
                    }
                }
                catch (Exception ex)
                {
                    Log.WriteError(ex.Message);
                    return false;
                }
            });

            return true;
        }

        void ReqTWRVersion()
        {
            ControlsEnabled(false);
            Task.Run(() =>
            {
                Thread.Sleep(100); // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."
                Send("{\"ep\":\"control\",\"cmd\":\"version\"}");
            });
        }

        #endregion

        void buttonSettings_Click(object sender, EventArgs e)
        {
            FormSettings FormSettings = new FormSettings();
            FormSettings.ShowDialog();

            m_AutoGetValuePeriodCounter = 0;
            m_AutoGetValuePeriod = Properties.Settings.Default.AutoGetValuePeriod;

            if (FormSettings.IsChanged)
                Restart();
        }

        void buttonReconnect_Click(object sender, EventArgs e)
        {
            Restart();
        }

        private void buttonSearch_Click(object sender, EventArgs e)
        {
            ControlsEnabled(false);
            listViewBus.Items.Clear();
            Send("{\"ep\":\"dallas\",\"cmd\":\"search\"}");
        }

        private void buttonGetValue_Click(object sender, EventArgs e)
        {
            GetValue();
        }

        private void checkBoxAutoGetValue_CheckedChanged(object sender, EventArgs e)
        {
            ControlsEnabled(!checkBoxAutoGetValue.Checked);
            if (!checkBoxAutoGetValue.Checked)
                return;
            m_AutoGetValuePeriodCounter = 0;
        }

        void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            Send("{\"ep\":\"dallas\",\"cmd\":\"close\"}", true);
            Thread.Sleep(100); // Wait, the packet shall be delivered. Response to this packet is not really interested.
            m_UDPClient?.Close();
        }

        void FormMain_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.C:
                    {
                        string StrData = "{\"values\":[";
                        for (int i = 0; i < listViewBus.Items.Count; ++i)
                        {
                            StrData += "{\"family_code\":\"" + listViewBus.Items[i].Text + "\",";
                            for (int si = 0; si < listViewBus.Items[i].SubItems.Count; ++si)
                            {
                                switch (si)
                                {
                                    case 1: StrData += "\"id\":\"" + listViewBus.Items[i].SubItems[si].Text + "\","; break;
                                    case 2: StrData += "\"value\":\"" + listViewBus.Items[i].SubItems[si].Text + "\""; break;
                                }
                            }

                            StrData += "}";
                            if (i != listViewBus.Items.Count - 1)
                                StrData += ",";
                        }
                        StrData += "]}";

                        if (listViewBus.Items.Count > 0)
                            Clipboard.SetText(StrData);
                        else
                            Clipboard.Clear();

                        break;
                    }
            }
        }
    }
}