namespace SergeM
{
    internal class ReceiverPolicyMTK_EB_800A : IReceiverPolicy
    {
        MsgsEnable m_MsgsEnabled = new();

        //public event EventHandler<NaviEventArgs>? NaviChanged;
        public event EventHandler<PortBaudrateEventArgs>? PortBaudrateChanged;

        public List<string> MakeOutMsgSetRestart(int baudrate)
        {
            List<string> Strs = new();
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PMTK011,MTKGPS"));
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PMTK010,001"));
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PMTK011,MTKGPS"));
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PMTK010,002"));
            return Strs;
        }

        public List<string> MakeOutMsgSetNavi()
        {
            DateTime TimeNow = DateTime.UtcNow;
            string timeNowStr = TimeNow.ToString("HHmmss") + ".000";
            string courseOverGround = "123.64";
            string speedOverGround = "0.53";

            List<string> Strs = new();
            if (m_MsgsEnabled.GGA.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNGGA," + timeNowStr + ",1122.334455,N,12233.445566,E,1,12,3.68,150.753,M,14.352,M,,"));
            if (m_MsgsEnabled.GLL.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNGLL,1122.334455,N,12233.445566,E," + timeNowStr + ",A,A"));
            if (m_MsgsEnabled.GSA.IsReady())
            {
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSA,A,3,11,12,13,14,15,16,17,,,,,,1.60,1.61,0.62"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GLGSA,A,3,65,66,67,,,,,,,,,,1.80,1.81,0.82"));
            }
            if (m_MsgsEnabled.GSV.IsReady())
            {
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,1,12,01,01,001,01,02,02,002,,03,03,003,,04,04,004,"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,2,12,05,05,005,05,06,06,006,,07,07,007,,08,08,008,08"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,3,12,09,09,009,09,10,10,010,,12,12,012,,13,13,013,13"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GLGSV,3,1,10,65,01,001,01,66,02,002,,67,03,003,,68,04,004,"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GLGSV,3,2,10,69,05,005,05,70,06,006,,71,07,007,,72,08,008,08"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GLGSV,3,3,10,73,09,009,09,74,10,010,10"));
            }
            if (m_MsgsEnabled.RMC.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNRMC," + timeNowStr + ",A,1122.334455,N,12233.445566,E,"+ speedOverGround + ","+ courseOverGround + "," + TimeNow.ToString("yyMMdd") + ",,,A"));
            if (m_MsgsEnabled.VTG.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNVTG,"+ courseOverGround + ",T,,M,"+ speedOverGround + ",N,"+ Units.KnotsToKmH(speedOverGround, 2) + ",K,A"));
            if (m_MsgsEnabled.ZDA.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNZDA," + timeNowStr + "," + TimeNow.ToString("dd") + "," + TimeNow.ToString("MM") + "," + TimeNow.ToString("yyyy") + ",,"));
            return Strs;
        }

        public List<string> HandleMsg(string msg, int baudrate)
        {
            string[] Payload = utils.ProtocolNMEA.GetPayload(msg);
            if (Payload.Length == 0)
                return new();

            if (Payload[0] == "PMTK000" && Payload.Length == 1)
                return MakeOutMsg("PMTK001,000,3");

            if (Payload[0] == "PMTK605" && Payload.Length == 1)
                return MakeOutMsg("PMTK705,AXN_0.2,1234,EB-800A,ABCD");

            if (Payload[0] == "PMTK104" && Payload.Length == 1)
                return MakeOutMsgSetRestart(0);

            if (Payload[0] == "PMTK314" && Payload.Length == 20)
            {
                m_MsgsEnabled.GGA.SetFrequency(Payload[1]);
                m_MsgsEnabled.GLL.SetFrequency(Payload[2]);
                m_MsgsEnabled.GSA.SetFrequency(Payload[3]);
                m_MsgsEnabled.GSV.SetFrequency(Payload[4]);
                m_MsgsEnabled.RMC.SetFrequency(Payload[5]);
                m_MsgsEnabled.VTG.SetFrequency(Payload[6]);
                m_MsgsEnabled.ZDA.SetFrequency(Payload[18]);
                return MakeOutMsg("PMTK001,314,3");
            }

            return new();
        }

        List<string> MakeOutMsg(string msg)
        {
            List<string> Strs = new();
            Strs.Add(utils.ProtocolNMEA.MakeMsg(msg));
            return Strs;
        }
    }
}
