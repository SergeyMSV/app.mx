using System.Net;

namespace SergeM
{
    public partial class FormMain : Form
    {
        utils.twr.UDPClientEpUART? m_TWRClient;
        Color m_buttonRecordColor;
        Color m_buttonStopReceiveColor;
        bool m_StopReceive;
        bool m_Recording = false;
        StreamWriter m_RecordingFile;
        readonly string m_Text;
        //bool m_StateCloseUponDisconnect = false;
        bool m_StateReconnect = false;
        //volatile bool m_StateSearch = false;

        public FormMain()
        {
            InitializeComponent();
            m_Text = Text;
            m_buttonRecordColor = buttonRecord.ForeColor;
            m_buttonStopReceiveColor = buttonStopReceive.ForeColor;
            m_Text = Text;
            Cursor = Cursors.WaitCursor;
            SetStateNotConnected();
            SetControlsState(false);
            InitializeTWRClient();
            _ = TWRClientOpen();

           // SetStateNotConnected();
           //
           // m_TWRClient.Connected += OnConnected;
           // m_TWRClient.Received += OnReceived;
           // Cursor = Cursors.WaitCursor;
           // Task.Run(() =>
           //     {
           //         Thread.Sleep(100); // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."
           //         if (TWRClientOpen())
           //             BeginInvoke(new Action(() => { ControlsEnabled(true); }));
           //         BeginInvoke(new Action(() => { Cursor = Cursors.Default; }));
           //     });
        }

        void InitializeTWRClient()
        {
            if (m_TWRClient != null)
                m_TWRClient.Dispose();
            string IPAddrStr = Properties.Settings.Default.Localhost ? "127.0.0.1" : Properties.Settings.Default.IPAddressRemote;
            IPEndPoint Ep = new(IPAddress.Parse(IPAddrStr), Properties.Settings.Default.UDPPortRemote);
            m_TWRClient = new(Properties.Settings.Default.UDPPortLocal, Ep, Properties.Settings.Default.Log, Properties.Settings.Default.UART, Properties.Settings.Default.UARTBaudrate);
            m_TWRClient.Connected += OnConnected;
            m_TWRClient.Opened += OnOpened;
            m_TWRClient.Closed += OnClosed;
            m_TWRClient.Received += OnReceived;
            m_TWRClient.Failed += OnFailed;
        }

        async Task<bool> TWRClientOpen()
        {
            if (m_TWRClient == null)
                return false;
            return await m_TWRClient.Open();
        }

        void OnOpened(object? sender, EventArgs e)
        {
            Cursor = Cursors.Default;
        }

        void OnClosed(object? sender, EventArgs e)
        {
            SetControlsState(false);
        }

        void OnConnected(object? sender, utils.twr.ConnectedEventArgs e)
        {
            BeginInvoke(new Action(() =>
            {
                Text = m_Text + ": " + e.Version + " (" + e.Endpoint.ToString() + ")";
            }));
        }

        //void ClearListViewItems()
        //{
        //    foreach (var i in listViewBus.Items)
        //    {
        //        ((ListViewItem)i).SubItems[2].Text = "";
        //    }
        //}

        void OnReceived(object? sender, utils.twr.UARTReceivedEventArgs e)
        {
            if (m_Recording && m_RecordingFile != null)
            {
                try
                {
                    m_RecordingFile.Write(e.Data);
                }
                catch { }
            }

            if (m_StopReceive)
                return;

            BeginInvoke(new Action(() =>
            {
                int SelectionStart = textBoxReceive.SelectionStart;
                int SelectionLength = textBoxReceive.SelectionLength;

                bool SelectedEnd = textBoxReceive.SelectionStart == textBoxReceive.Text.Length;

                textBoxReceive.SuspendLayout();

                try
                {
                    const int SizeMax = 4096; // [#]
                    if (textBoxReceive.Text.Length > SizeMax * 2)
                    {
                        int SizeRemove = textBoxReceive.Text.Length - SizeMax;
                        textBoxReceive.Text = textBoxReceive.Text[SizeRemove..]; // C# 8.0+
                    }

                    textBoxReceive.Text += e.Data;
                    textBoxReceive.Select(SelectionStart, SelectionLength);
                }
                finally
                {
                    textBoxReceive.ResumeLayout();
                }

                if (textBoxReceive.SelectionLength != 0 || !SelectedEnd)
                    return;
                textBoxReceive.SelectionStart = textBoxReceive.Text.Length;
                textBoxReceive.SelectionLength = 0;
                textBoxReceive.ScrollToCaret();
            }));
        }

        void OnFailed(object? sender, utils.twr.FailedEventArgs e)
        {
            SetStateRequestFailed(e.Msg);
            //ClearListViewItems();
        }

        void SetControlsState(bool reqState, bool errState)
        {
            bool twrOpen = m_TWRClient != null && m_TWRClient.IsOpen;
            if (!twrOpen || errState)
            {
                buttonSearch.Enabled = false;
                buttonGetValue.Enabled = false;
                checkBoxAutoGetValue.Enabled = false;
                return;
            }

            if (!checkBoxAutoGetValue.Checked)
            {
                checkBoxAutoGetValue.Enabled = reqState;
            }
            else if (reqState)
            {
                checkBoxAutoGetValue.Enabled = true;
            }
            bool en = reqState && !checkBoxAutoGetValue.Checked;
            buttonSearch.Enabled = en;
            buttonGetValue.Enabled = en;
        }
        void SetControlsStateError() => SetControlsState(false, true);
        void SetControlsState(bool state) => SetControlsState(state, false);

        void SetStateNotConnected()
        {
            listViewBus.Items.Clear();
            Text = m_Text + ": not connected";
            SetControlsState(false);
        }

        void SetStateRequestComplete()
        {
            SetControlsState(true);
            Cursor = Cursors.Default;
        }

        void SetStateRequestFailed(string errMsg)
        {
            SetControlsStateError();
            Cursor = Cursors.Default;
            MessageBoxShow("Error: " + errMsg);
        }

        void MessageBoxShow(string msg)
        {
            if (m_MessageBoxIsShown)
                return;
            m_MessageBoxIsShown = true;
            MessageBox.Show(msg);
            m_MessageBoxIsShown = false;
        }

        async Task Reconnect()
        {
            if (m_StateReconnect || m_closingAfterDisconnect)
                return;
            m_StateReconnect = true;
            try
            {
                Cursor = Cursors.WaitCursor;
                SetStateNotConnected();
                if (m_TWRClient != null && m_TWRClient.IsOpen)
                    await m_TWRClient.Close(1000);

                InitializeTWRClient();
                if (!await TWRClientOpen())
                    Cursor = Cursors.Default;
            }
            catch (Exception ex)
            {
                Cursor = Cursors.Default;
                Log.WriteError(ex.Message);
            }
            finally
            {
                m_reconnecting = false;
            }
        }


        //---------------------------------------






        bool TWRClientOpen()
        {
            string IPAddrStr = Properties.Settings.Default.Localhost ? "127.0.0.1" : Properties.Settings.Default.IPAddressRemote;
            IPEndPoint Ep = new(IPAddress.Parse(IPAddrStr), Properties.Settings.Default.UDPPortRemote);
            return m_TWRClient.Open(Properties.Settings.Default.UDPPortLocal, Ep, Properties.Settings.Default.Log, Properties.Settings.Default.UART, Properties.Settings.Default.UARTBaudrate);
        }

        void OnConnected(object? sender, utils.twr.ConnectedEventArgs e)
        {
            BeginInvoke(new Action(() =>
            {
                Text = m_Text + ": " + e.Version + " (" + e.Endpoint.ToString() + ", " + Properties.Settings.Default.UART + ")";
            }));
        }

        void OnReceived(object? sender, utils.twr.UARTReceivedEventArgs e)
        {
            if (m_Recording && m_RecordingFile != null)
            {
                try
                {
                    m_RecordingFile.Write(e.Data);
                }
                catch { }
            }

            if (m_StopReceive)
                return;

            BeginInvoke(new Action(() =>
            {
                int SelectionStart = textBoxReceive.SelectionStart;
                int SelectionLength = textBoxReceive.SelectionLength;

                bool SelectedEnd = textBoxReceive.SelectionStart == textBoxReceive.Text.Length;

                textBoxReceive.SuspendLayout();

                try
                {
                    const int SizeMax = 4096; // [#]
                    if (textBoxReceive.Text.Length > SizeMax * 2)
                    {
                        int SizeRemove = textBoxReceive.Text.Length - SizeMax;
                        textBoxReceive.Text = textBoxReceive.Text[SizeRemove..]; // C# 8.0+
                    }

                    textBoxReceive.Text += e.Data;
                    textBoxReceive.Select(SelectionStart, SelectionLength);
                }
                finally
                {
                    textBoxReceive.ResumeLayout();
                }

                if (textBoxReceive.SelectionLength != 0 || !SelectedEnd)
                    return;
                textBoxReceive.SelectionStart = textBoxReceive.Text.Length;
                textBoxReceive.SelectionLength = 0;
                textBoxReceive.ScrollToCaret();
            }));
        }

        void ControlsEnabled(bool state)
        {
            buttonSend.Enabled = state;
        }

        void SetStateNotConnected()
        {
            Text = m_Text + ": not connected";
            ControlsEnabled(false);
        }

        void Reconnect()
        {
            Cursor = Cursors.WaitCursor;
            SetStateNotConnected();
            m_TWRClient.Close();
            ControlsEnabled(TWRClientOpen());
            Cursor = Cursors.Default;
        }

        private void buttonSettings_Click(object sender, EventArgs e)
        {
            FormSettings FormSettings = new();
            FormSettings.ShowDialog();
            if (FormSettings.IsPortSettingsChanged)
                Reconnect();
            m_TWRClient.LogEnabled = Properties.Settings.Default.Log;
        }

        private void buttonReconnect_Click(object sender, EventArgs e) => Reconnect();
        
        private void buttonSend_Click(object sender, EventArgs e)
        {
            if (Properties.Settings.Default.Protocol == "NMEA")
            {
                string CRC = utils.ProtocolNMEA.GetCRC(textBoxSend.Text);
                m_TWRClient.Send("$" + textBoxSend.Text + "*" + CRC + "\r\n");
                return;
            }
            m_TWRClient.Send(textBoxSend.Text + "\r\n");
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_TWRClient.Connected -= OnConnected;
            m_TWRClient.Received -= OnReceived;
            m_TWRClient.Close();
        }

        private void buttonStopReceive_Click(object sender, EventArgs e)
        {
            m_StopReceive = !m_StopReceive;
            buttonStopReceive.ForeColor = m_StopReceive ? Color.Red : m_buttonStopReceiveColor;
        }

        private void buttonRecord_Click(object sender, EventArgs e)
        {
            m_Recording = !m_Recording;
            buttonRecord.ForeColor = m_Recording ? Color.Red : m_buttonRecordColor;
            try
            {
                if (m_Recording)
                {
                    DateTime UTC = DateTime.UtcNow;
                    string FileName = "Record " + UTC.ToString("yyyy-MM-dd") + ".txt";
                    bool FileNew = !File.Exists(FileName);
                    m_RecordingFile = new StreamWriter(FileName, true);
                    string StartLine = (FileNew ? "" : "\n") + "--------- START, UTC: " + UTC.ToString("yyyy-MM-dd HH:mm:ss") + ", port: " + Properties.Settings.Default.UART;
                    m_RecordingFile.WriteLine(StartLine);
                }
                else
                {
                    m_RecordingFile.Close();
                }
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }
    }
}
