using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;

namespace SergeM
{
    public partial class FormMain : Form
    {
        UdpClient? m_UDPClient;
        Color m_buttonStopReceiveColor;
        string m_TWREndPoint;
        bool m_StopReceive;
        CancellationTokenSource m_ReceiveRequestsStopped;
        ManualResetEvent m_ReceiveClosed = new ManualResetEvent(false);
        readonly string m_Text;

        public FormMain()
        {
            InitializeComponent();
            m_Text = Text;
            m_buttonStopReceiveColor = buttonStopReceive.ForeColor;
            Restart();
        }

        void ControlsEnabled(bool state)
        {
            Cursor = state ? Cursors.Default : Cursors.WaitCursor;
            buttonSend.Enabled = state;
        }

        void SetStateNotConnected()
        {
            Text = m_Text + ": not connected";
            ControlsEnabled(false);
            Cursor = Cursors.Default;
        }

        void Restart()
        {
            SetStateNotConnected();
            CloseTWRPort();
            Cursor = Cursors.WaitCursor;
            if (!RestartUDP())
                return;
            ReqTWRVersion();
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

            m_ReceiveRequestsStopped = new CancellationTokenSource();

            //Task.Run(() =>
            //{
            //    try
            //    {
            //        while (true)//!m_ReceiveCts.Token.IsCancellationRequested)
            //        {
            //            IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
            //            Byte[] ReceiveBytes = m_UDPClient.Receive(ref RemoteIpEndPoint);
            //            string RecvStr = Encoding.Default.GetString(ReceiveBytes);
            //            PacketDecoder(RemoteIpEndPoint, RecvStr);
            //            //Invoke(new Action(() => PacketDecoder(RemoteIpEndPoint, RecvStr))); // [TBD] is it OK ?
            //        }
            //    }
            //    catch (Exception ex)
            //    {
            //        Log.WriteError(ex.Message);
            //    }
            //}
            //);


            Task.Run(async () => // [*] Øt doesn't increase amount of threads for some unknown reason.
            {
                try
                {
                    while (true)
                    {
                        UdpReceiveResult RecvResult = await m_UDPClient.ReceiveAsync();
                        string RecvStr = Encoding.Default.GetString(RecvResult.Buffer);
                        PacketDecoder(RecvResult.RemoteEndPoint, RecvStr);
                    }
                }
                catch (Exception ex)
                {
                    Log.WriteError(ex.Message);
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

        void PacketDecoder(IPEndPoint ep, string rsp)
        {
            try
            {
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
                            Invoke(new Action(() =>
                            {
                                m_TWREndPoint = Properties.Settings.Default.UART;
                                Text = m_Text + ": " + Ver + " (" + ep.ToString() + ")";
                                ControlsEnabled(false);
                            }));
                            SendCmd("open");
                            break;
                        }
                    case "open":
                        {
                            if (Response != "ok")
                            {
                                Invoke(new Action(() => { ControlsEnabled(false); }));
                                break;
                            }

                            Invoke(new Action(() => { ControlsEnabled(true); }));

                            Task.Run(() => // [TBD] it's not a good idea !!!
                            {
                                while (!m_ReceiveRequestsStopped.Token.IsCancellationRequested)
                                {
                                    Thread.Sleep(100); // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."
                                    SendCmd("receive");
                                }
                            });
                            break;
                        }
                    case "close": // It means that the existed connection already closed.
                        {
                            m_TWREndPoint = "";
                            m_ReceiveClosed.Set();
                            break;
                        }
                    case "receive":
                        {
                            if (m_StopReceive || m_ReceiveRequestsStopped.Token.IsCancellationRequested)
                                break;
                            string Data = Node?["data"]?.ToString()! ?? "";
                            if (Data.Length == 0)
                                break;

                            BeginInvoke(new Action(() =>
                            {
                                int SelectionStart = textBoxReceive.SelectionStart;
                                int SelectionLength = textBoxReceive.SelectionLength;

                                bool SelectedEnd = textBoxReceive.SelectionStart == textBoxReceive.Text.Length;

                                textBoxReceive.SuspendLayout();

                                try
                                {
                                    const int SizeMax = 4096;
                                    if (textBoxReceive.Text.Length > SizeMax * 2)
                                    {
                                        int SizeRemove = textBoxReceive.Text.Length - SizeMax;
                                        textBoxReceive.Text = textBoxReceive.Text[SizeRemove..]; // C# 8.0+
                                    }

                                    textBoxReceive.Text += Data;
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
                            break;
                        }
                    case "send": // Data has been sent and delivered successfully.
                        {
                            break;
                        }
                }
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        bool Send(string msg)
        {
            if (m_UDPClient == null)
                return false;

            try
            {
                string IPAddrStr = Properties.Settings.Default.Localhost ? "127.0.0.1" : Properties.Settings.Default.IPAddressRemote;
                IPEndPoint Ep = new IPEndPoint(IPAddress.Parse(IPAddrStr), Properties.Settings.Default.UDPPortRemote);

                Byte[] Req = Encoding.ASCII.GetBytes(msg);
                m_UDPClient?.Send(Req, Req.Length, Ep);
            }
            catch { }
            return true;
        }

        bool SendCmd(string cmd, string data)
        {
            if (m_TWREndPoint == null || m_TWREndPoint.Length == 0)
                return false;
            string Str = "{\"ep\":\"" + m_TWREndPoint + "\",\"cmd\":\"" + cmd + "\"";
            if (data.Length > 0)
                Str += ",\"data\":\"" + data + "\\r\\n\"";
            Str += "}";
            return Send(Str);
        }

        bool SendCmd(string cmd)
        {
            return SendCmd(cmd, "");
        }

        void CloseTWRPort()
        {
            m_ReceiveRequestsStopped?.Cancel();
            m_ReceiveClosed.Reset();
            if (SendCmd("close") && !m_ReceiveClosed.WaitOne(1000)) // [#] 1 sec
                Log.WriteError("TWR port probably hasn't been closed.");
        }

        private void buttonSettings_Click(object sender, EventArgs e)
        {
            FormSettings FormSettings = new FormSettings();
            FormSettings.ShowDialog();
            if (FormSettings.IsPortSettingsChanged)
                Restart();
        }

        private void buttonReconnect_Click(object sender, EventArgs e)
        {
            Restart();
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            if (Properties.Settings.Default.Protocol == "NMEA")
            {
                string CRC = utils.ProtocolNMEA.GetCRC(textBoxSend.Text);
                SendCmd("send", "$" + textBoxSend.Text + "*" + CRC);
                return;
            }

            SendCmd("send", textBoxSend.Text);
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            CloseTWRPort();
            m_UDPClient?.Close();
        }

        private void buttonStopReceive_Click(object sender, EventArgs e)
        {
            if (m_StopReceive)
            {
                m_StopReceive = false;
                buttonStopReceive.ForeColor = m_buttonStopReceiveColor;
            }
            else
            {
                m_StopReceive = true;
                buttonStopReceive.ForeColor = Color.Red;
            }
        }
    }
}
