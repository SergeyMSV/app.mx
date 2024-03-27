using System.Diagnostics;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Text.Json.Nodes;

namespace DallasRW
{
    public partial class FormMain : Form
    {
        public FormMain()
        {
            InitializeComponent();

            // [TBD] если в настройках "127.0.0.1" - запустить свой, если реальный - коннектиться к нему
            Init();
        }

        private Process m_DallasProcess;
        private UdpClient m_DallasUDPClient;

        private void DallasDecoder(IPEndPoint ep, string rsp)
        {
            utils.Log.WriteTrace("Receive from " + ep.ToString() + " " + rsp);

            //string Message = Encoding.UTF8.GetString(DataRaw);
            JsonNode Node = JsonNode.Parse(rsp)!;
            JsonNode NodeCmd = Node!["cmd"]!;
            JsonNode NodeStatus = Node!["rsp"]!;

            if (NodeStatus != null && NodeStatus.ToString() != "ok")
                MessageBox.Show("Response: " + NodeStatus.ToString() + ".");

            switch (NodeCmd.ToString())
            {
                case "read_key":
                    break;
                case "write_key":
                    break;
            }

            // [TBD] Parse read_key
        }

        private void DallasUDP() // and UI too
        {
            m_DallasUDPClient = new UdpClient(Properties.Settings.Default.UDPPortLocal);
            m_DallasUDPClient.EnableBroadcast = false;
            m_DallasUDPClient.DontFragment = true;

            Task.Run(async () => // [*] it doesn't increase amount of threads for some unknown reason
            {
                try
                {
                    while (true)
                    {
                        UdpReceiveResult RecvResult = await m_DallasUDPClient.ReceiveAsync();
                        string RecvStr = Encoding.Default.GetString(RecvResult.Buffer);
                        //utils.Log.WriteTrace("Receive from " + RecvResult.RemoteEndPoint.ToString() + " " + RecvStr);
                        Invoke(new Action(() => DallasDecoder(RecvResult.RemoteEndPoint, RecvStr))); // [TBD] is it OK ?

                        //utils.Log.WriteTrace("Receive from " + RecvResult.RemoteEndPoint.ToString(), RecvResult.Buffer);
                    }
                }
                catch (Exception ex)
                {
                    utils.Log.WriteError(ex.Message);
                }
            });
        }

        private void Init()
        {
            ProcessStartInfo StartInfo = new ProcessStartInfo();
            StartInfo.CreateNoWindow = true;
            StartInfo.FileName = "mxdls.exe";
            StartInfo.UseShellExecute = false;
            StartInfo.WindowStyle = ProcessWindowStyle.Hidden;
            //StartInfo.WorkingDirectory

            bool CloseApp = false;

            try
            {
                m_DallasProcess = Process.Start(StartInfo);
                Thread.Sleep(100); // in order for HasExited to be defined
                if (m_DallasProcess!.HasExited)
                {
                    CloseApp = true;
                    string Msg = "Process " + StartInfo.FileName + " hasn't been started.";
                    MessageBox.Show(Msg);
                    throw new Exception(Msg);
                }
                DallasUDP();

                buttonSend.Enabled = true;
            }
            catch (Exception ex)
            {
                utils.Log.WriteError(ex.Message);
            }

            if (CloseApp)
                Close();
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            if (m_DallasUDPClient == null)
                return;
            //Byte[] LocatorRequest = Encoding.ASCII.GetBytes("{\"cmd\":\"version\"}");
            Byte[] LocatorRequest = Encoding.ASCII.GetBytes("{\"cmd\":\"search\"}");
            //Byte[] LocatorRequest = Encoding.ASCII.GetBytes("{\"cmd\":\"read_key\"}");
            //Byte[] LocatorRequest = Encoding.ASCII.GetBytes("{\"cmd\":\"write_key\",\"id\":\"00000308c4de\"}");
            m_DallasUDPClient.Send(LocatorRequest, LocatorRequest.Length, new IPEndPoint(IPAddress.Parse("127.0.0.1"), Properties.Settings.Default.UDPPortRemote));
        }

        private void buttonRead_Click(object sender, EventArgs e)
        {
            if (m_DallasUDPClient == null)
                return;

            Byte[] LocatorRequest = Encoding.ASCII.GetBytes("{\"cmd\":\"read_key\"}");
            m_DallasUDPClient.Send(LocatorRequest, LocatorRequest.Length, new IPEndPoint(IPAddress.Parse("127.0.0.1"), Properties.Settings.Default.UDPPortRemote));
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (m_DallasProcess != null)
                m_DallasProcess.Kill();
            if (m_DallasUDPClient != null)
                m_DallasUDPClient.Close();
        }
    }
}