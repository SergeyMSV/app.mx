using System.Net;
using System.Text.Json.Nodes;

namespace utils.twr
{
    public class UDPClientEpUART : UDPClient
    {
        public event EventHandler<EventArgs>? Opened;
        public event EventHandler<EventArgs>? Closed;
        public event EventHandler<UARTReceivedEventArgs>? Received;
        public event EventHandler<FailedEventArgs>? Failed;

        public UDPClientEpUART(ushort udpPortLocal, IPEndPoint udpEndpointRemote, bool logEnabled, string uartID, UInt32 uartBR)
            : base(udpPortLocal, udpEndpointRemote, logEnabled, uartID, uartBR)
        {

        }

        protected override Task ErrorOccured(string msg)
        {
            Failed?.Invoke(this, new(msg));
            return Task.CompletedTask;
        }

        protected override async Task ReceivedCmdOpen()
        {
            await Send(Cmds.MakeDallasSearch(m_TWREndpoint));
            Opened?.Invoke(this, new()); // [TBD] it shall be called upon successful search
        }

        protected override Task ReceivedCmdClose()
        {
            Closed?.Invoke(this, new());
            return Task.CompletedTask;
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
                            string Data = rspJson["data"]?.ToString() ?? "";
                            string Datafff = rspJson["datafff"]?.ToString() ?? ""; // [TBD] for test
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
    }
}
