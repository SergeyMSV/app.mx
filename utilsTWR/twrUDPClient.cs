using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;

namespace utils.twr
{
    public abstract class UDPClient
    {
        public event EventHandler<ConnectedEventArgs>? Connected;

        UdpClient? m_UDPClient;
        object m_UDPClientSendLock = new();
        protected CancellationTokenSource? m_ReceiveCancellaton;
        protected object m_ReceiveCancellatonLock = new();
        ManualResetEvent m_WaitForOpen = new(false);
        ManualResetEvent m_WaitForClose = new(false);
        protected ManualResetEvent m_WaitForReceivingStop = new(false);
        IPEndPoint? m_EnpointRemote;
        protected string m_TWREndpoint = ""; // [TEST]
        protected UInt32 m_TWREndpointUARTBaudrate = 0; // [TEST]

        public bool IsOpen { get; private set; } = false;

        public bool LogEnabled { get; set; } = false;

        protected bool OpenInternal(ushort udpPortLocal, IPEndPoint udpEndpointRemote, bool logEnabled, string uartID, UInt32 uartBR)
        {
            m_EnpointRemote = udpEndpointRemote;
            LogEnabled = logEnabled;
            m_TWREndpoint = uartID;
            m_TWREndpointUARTBaudrate = uartBR;
            return OpenInternal(udpPortLocal);
        }

        bool OpenInternal(ushort udpPortLocal)
        {
            try
            {
                m_UDPClient?.Close();
                m_UDPClient = new UdpClient(udpPortLocal)
                {
                    EnableBroadcast = false,
                    DontFragment = true,
                };
            }
            catch (Exception ex)
            {
                LogWriteError(ex.Message);
                return false;
            }

            lock (m_ReceiveCancellatonLock)
            {
                if (m_ReceiveCancellaton != null)
                {
                    m_ReceiveCancellaton.Cancel();
                    m_ReceiveCancellaton.Dispose();
                }
                m_ReceiveCancellaton = new CancellationTokenSource();
            }

            //Task.Run(() =>
            //{
            //    try
            //    {
            //        while (true)//!m_ReceiveCts.Token.IsCancellationRequested)
            //        {
            //            IPEndPoint RemoteIpEndPoint = new IPEndPoint(IPAddress.Any, 0);
            //            Byte[] ReceiveBytes = m_UDPClient.Receive(ref RemoteIpEndPoint);
            //            string RecvStr = Encoding.Default.GetString(ReceiveBytes);
            //            Received?.Invoke(this, new(RecvResult.RemoteEndPoint, RecvStr));
            //            //PacketDecoder(RemoteIpEndPoint, RecvStr);
            //            //Invoke(new Action(() => PacketDecoder(RemoteIpEndPoint, RecvStr))); // [TBD] is it OK ?
            //        }
            //    }
            //    catch (Exception ex)
            //    {
            //        LogWriteError(ex.Message);
            //    }
            //}
            //);

            Task.Run(async () => // [*] It doesn't increase amount of threads for some unknown reason.
            {
                try
                {
                    while (true)
                    {
                        UdpReceiveResult RecvResult = await m_UDPClient.ReceiveAsync(m_ReceiveCancellaton.Token);
                        string RecvStr = Encoding.Default.GetString(RecvResult.Buffer);
                        PacketDecoder(RecvResult.RemoteEndPoint, RecvStr);
                    }
                }
                catch (OperationCanceledException)
                {
                }
                catch (Exception ex)
                {
                    LogWriteError(ex.Message);
                }
            });

            m_WaitForOpen.Reset();
            SendInternal(Cmds.MakeGetVersion());
            return m_WaitForOpen.WaitOne(1000); // [#] 1 sec
        }

        public bool Close()
        {
            IsOpen = false;
            m_WaitForClose.Reset();
            bool Res = SendInternal(Cmds.MakeClose(m_TWREndpoint));
            if (Res)
                Res = m_WaitForClose.WaitOne(1000); // [#] 1 sec
            m_WaitForReceivingStop.Reset();
            m_ReceiveCancellaton?.Cancel();
            m_WaitForReceivingStop.WaitOne(1000); // [#] 1 sec
            return Res;
        }

        protected abstract void ReceivedCmdOpen();
        protected abstract void ReceivedCmd(IPEndPoint ep, JsonNode rspJson);

        void PacketDecoder(IPEndPoint ep, string rsp)
        {
            try
            {
                LogWriteTrace("Received from " + ep.ToString() + "\n" + rsp);

                JsonNode Node = JsonNode.Parse(rsp)!;
                JsonNode NodeCmd = Node?["cmd"] ?? "unknown";
                JsonNode NodeResponse = Node!["rsp"]!;

                string Response = NodeResponse == null ? "" : NodeResponse.ToString();

                switch (NodeCmd.ToString())
                {
                    case "version":
                        {
                            string Ver = Node?["version"]?.ToString()! ?? "unknown";
                            Connected?.Invoke(this, new(ep, Ver));
                            SendInternal(twr.Cmds.MakeOpen(m_TWREndpoint, m_TWREndpointUARTBaudrate));
                            break;
                        }
                    case "open":
                        {
                            IsOpen = Response == "ok";
                            if (!IsOpen)
                                break;
                            m_WaitForOpen.Set();
                            ReceivedCmdOpen();
                            break;
                        }
                    case "close": // It means that the existed connection already closed.
                        {
                            IsOpen = false;
                            m_WaitForClose.Set();
                            break;
                        }
                    default:
                        {
                            ReceivedCmd(ep, Node);
                            break;
                        }
                }
            }
            catch (Exception ex)
            {
                LogWriteError(ex.Message);
            }
        }

        protected bool SendInternal(string msg)
        {
            if (m_UDPClient == null)
                return false;
            try
            {
                Byte[] Req = Encoding.ASCII.GetBytes(msg);
                lock (m_UDPClientSendLock)
                {
                    m_UDPClient?.Send(Req, Req.Length, m_EnpointRemote);
                }
                LogWriteTrace("Sent to " + (m_EnpointRemote?.ToString() ?? "unknown endpoint") + " " + msg);
            }
            catch { }
            return true;
        }

        protected void LogWriteError(string message)
        {
            //if (LogEnabled)
            Log.WriteError(message);
        }

        protected void LogWriteTrace(string message)
        {
            if (LogEnabled)
                Log.WriteTrace(message);
        }
    }
}
