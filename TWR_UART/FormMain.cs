using System.Net;

namespace SergeM
{
    public partial class FormMain : Form
    {
        utils.twr.UDPClientEndpointUART m_TWRClient = new();
        Color m_buttonRecordColor;
        Color m_buttonStopReceiveColor;
        bool m_StopReceive;
        bool m_Recording = false;
        StreamWriter m_RecordingFile;
        readonly string m_Text;

        public FormMain()
        {
            InitializeComponent();
            m_Text = Text;
            m_buttonRecordColor = buttonRecord.ForeColor;
            m_buttonStopReceiveColor = buttonStopReceive.ForeColor;

            SetStateNotConnected();

            m_TWRClient.Connected += OnConnected;
            m_TWRClient.Received += OnReceived;
            Cursor = Cursors.WaitCursor;
            Task.Run(() =>
                {
                    Thread.Sleep(100); // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."
                    if (TWRClientOpen())
                        BeginInvoke(new Action(() => { ControlsEnabled(true); }));
                    BeginInvoke(new Action(() => { Cursor = Cursors.Default; }));
                });
        }

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
