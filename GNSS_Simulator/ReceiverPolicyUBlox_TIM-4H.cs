namespace SergeM
{
    internal class ReceiverPolicyUBlox_TIM_4H : IReceiverPolicy
    {
        MsgsEnable m_MsgsEnabled = new();

        //public event EventHandler<NaviEventArgs>? NaviChanged;
        public event EventHandler<PortBaudrateEventArgs>? PortBaudrateChanged;

        public List<string> MakeOutMsgSetRestart(int baudrate)
        {
            List<string> Strs = new();
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("GPTXT,01,01,02,u-blox ag - www.u-blox.com"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("GPTXT,01,01,02,ANTARIS ATR123x HW 12345678"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("GPTXT,01,01,02,EXT CORE       7.00    Sep 02 2026 16:45:32"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("GPTXT,01,01,02,INT EXT0 (RCV) M9H5.3  Sep 02 2026 20:43:54"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("GPTXT,01,01,02,LIC 1234-5678-90AB-CDEF-7654"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("GPTXT,01,01,02,QDS 12345678901"));
            return Strs;
        }

        public List<string> MakeOutMsgSetNavi()
        {
            DateTime TimeNow = DateTime.UtcNow;
            string timeNowStr = TimeNow.ToString("HHmmss") + ".00";
            string posLatLon = "1122.33445,N,12233.44556,E";
            string speedOverGround = "20.78";
            string courseOverGround = "2.227";
            List<string> Strs = new();
            if (m_MsgsEnabled.GGA.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGGA," + timeNowStr + "," + posLatLon + ",1,03,11.57,639.3,M,14.3,M,,"));
            if (m_MsgsEnabled.GLL.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNGLL," + posLatLon + "," + timeNowStr + ",A,A"));
            if (m_MsgsEnabled.GSA.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSA,A,3,07,10,13,21,16,,,,,,,,11.30,0.7,1.00"));
            if (m_MsgsEnabled.GSV.IsReady())
            {
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,1,12,01,01,001,01,02,02,002,,03,03,003,,04,04,004,"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,2,12,05,05,005,05,06,06,006,,07,07,007,,08,08,008,08"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,3,12,09,09,009,09,10,10,010,,12,12,012,,13,13,013,13"));
            }
            if (m_MsgsEnabled.RMC.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNRMC," + timeNowStr + ",A," + posLatLon + "," + speedOverGround + "," + courseOverGround + "," + TimeNow.ToString("yyMMdd") + ",,,A"));
            if (m_MsgsEnabled.VTG.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNVTG," + courseOverGround + ",T,,M," + speedOverGround + ",N," + Units.KnotsToKmH(speedOverGround, 3) + ",K,A"));
            if (m_MsgsEnabled.ZDA.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPZDA," + timeNowStr + "," + TimeNow.ToString("dd") + "," + TimeNow.ToString("MM") + "," + TimeNow.ToString("yyyy") + ",,"));
            return Strs;
        }

        public List<string> HandleMsg(string msg, int baudrate)
        {
            return new();
        }
    }
}
