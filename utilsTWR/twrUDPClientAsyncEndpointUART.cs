using System.Net;
using System.Text.Json.Nodes;

namespace utils.twr
{
    /*public class UDPClientAsyncEndpointUART : UDPClientAsync
    {
        public event EventHandler<UARTReceivedEventArgs>? Received;

        public async Task<bool> Open(ushort udpPortLocal, IPEndPoint udpEndpointRemote, bool logEnabled, string uartID, UInt32 uartBR)
            => await OpenInternal(udpPortLocal, udpEndpointRemote, logEnabled, uartID, uartBR);

        public async Task<bool> Send(string msg)
        {
            //if (!IsOpen)
                return false;
            //return await SendInternal(Cmds.MakeSend(m_TWREndpoint, msg));
        }

        protected override async Task ReceivedCmdOpen()
        {
            //Task.Run(() =>
            //{
            //    while (true)
            //    {
            //        //lock (m_ReceiveCancellatonLock)
            //       // {
            //            //if (m_ReceiveCancellaton == null || m_ReceiveCancellaton.Token.IsCancellationRequested)
            //            //    break;
            //        //}
            //        Thread.Sleep(200); // [#]
            //       // SendInternal(Cmds.MakeReceive(m_TWREndpoint));
            //    }
            //    //m_WaitForReceivingStop.Set();
            //    LogWriteTrace("Receiving thread closed.");
            //});
        }

        protected override void ReceivedCmd(IPEndPoint ep, JsonNode rspJson)
        {
            try
            {
                JsonNode NodeCmd = rspJson["cmd"] ?? "unknown";
                switch (NodeCmd.ToString())
                {
                    case "receive":
                        {
                            //lock (m_ReceiveCancellatonLock)
                           // {
                                //if (m_ReceiveCancellaton == null || m_ReceiveCancellaton.Token.IsCancellationRequested)
                                //    break;
                            //}
                            string Data = rspJson?["data"]?.ToString()! ?? "";
                            if (Data.Length == 0)
                                break;
                            Received?.Invoke(this, new(Data));
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
                LogWriteError(ex.Message);
            }
        }
    }*/
}
