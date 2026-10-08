using System.Net;
using System.Text.Json.Nodes;

namespace utils.twr
{
    public class UDPClientEpDallas : UDPClient
    {
        public event EventHandler<EventArgs>? Opened;
        public event EventHandler<EventArgs>? Closed;
        public event EventHandler<DallasSearchedEventArgs>? Searched;
        public event EventHandler<DallasReceivedEventArgs>? Received;

        public UDPClientEpDallas(ushort udpPortLocal, IPEndPoint udpEndpointRemote, bool logEnabled)
            : base(udpPortLocal, udpEndpointRemote, logEnabled, "dallas", 0)
        {

        }

        public async Task<bool> SendSearch() => await Send(Cmds.MakeDallasSearch(m_TWREndpoint));

        public async Task<bool> SendGetThermo(List<string> ids)
        {
            if (!IsOpen)
                return false;
            return await Send(Cmds.MakeDallasGetThermo(m_TWREndpoint, ids));
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
                    case "search":
                        {
                            Dictionary<string, string> ROMs = new();
                            if (rspJson["roms"] is JsonArray NodeROMs)
                            {
                                foreach (var rom in NodeROMs)
                                {
                                    if (rom == null)
                                        continue;
                                    JsonNode? NodeFamilyCode = rom["family_code"];
                                    JsonNode? NodeID = rom["id"];
                                    if (NodeFamilyCode == null || NodeID == null)
                                        continue;
                                    ROMs[NodeID.ToString()] = NodeFamilyCode.ToString();
                                }
                            }
                            Searched?.Invoke(this, new(ROMs));
                            break;
                        }
                    case "thermo":
                        {
                            Dictionary<string, string> Data = new();
                            if (rspJson["measurements"] is JsonArray NodeValues)
                            {
                                foreach (var node in NodeValues)
                                {
                                    if (node == null)
                                        continue;
                                    JsonNode? NodeID = node["id"];
                                    if (NodeID == null) // the temperature may be equal to null
                                        continue;
                                    Data[NodeID.ToString()] = node["temperature"]?.ToString() ?? string.Empty;
                                }
                            }
                            Received?.Invoke(this, new(Data));
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
