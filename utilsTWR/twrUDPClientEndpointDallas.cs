using System.Net;
using System.Text.Json.Nodes;

namespace utils.twr
{
    public class UDPClientEndpointDallas : UDPClient
    {
        public event EventHandler<DallasSearchedEventArgs>? Searched;
        public event EventHandler<DallasReceivedEventArgs>? Received;

        public bool Open(ushort udpPortLocal, IPEndPoint udpEndpointRemote, bool logEnabled)
            => OpenInternal(udpPortLocal, udpEndpointRemote, logEnabled, "dallas", 0);

        public bool SendSearch()
        {
            if (!IsOpen)
                return false;
            return SendInternal(Cmds.MakeDallasSearch(m_TWREndpoint));
        }

        public bool SendGetThermo(List<string> ids)
        {
            if (!IsOpen)
                return false;
            return SendInternal(Cmds.MakeDallasGetThermo(m_TWREndpoint, ids));
        }

        protected override void ReceivedCmdOpen()
        {
            //SendInternal(Cmds.MakeDallasSearch());
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
                            JsonNode NodeROMs = rspJson["roms"]!;
                            if (NodeROMs != null)
                            {
                                foreach (var rom in NodeROMs.AsArray())
                                {
                                    if (rom == null)
                                        continue;
                                    JsonNode NodeFamilyCode = rom["family_code"]!;
                                    JsonNode NodeID = rom["id"]!;
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
                            JsonNode NodeValues = rspJson["measurements"]!;
                            if (NodeValues != null)
                            {
                                foreach (var node in NodeValues.AsArray())
                                {
                                    if (node == null)
                                        continue;
                                    JsonNode NodeID = node["id"]!;
                                    JsonNode NodeTemperature = node["temperature"]!;
                                    if (NodeID == null) // || NodeTemperature == null) the temperature may be equal to null
                                        continue;
                                    Data[NodeID.ToString()] = NodeTemperature.ToString();
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
