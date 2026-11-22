namespace SergeM
{
    internal class ReceiverPolicySiRF_LR9548S : IReceiverPolicy
    {
        MsgsEnable m_MsgsEnabled = new();

        //public event EventHandler<NaviEventArgs>? NaviChanged;
        public event EventHandler<PortBaudrateEventArgs>? PortBaudrateChanged;

        public List<string> MakeOutMsgSetRestart(int baudrate)
        {
            List<string> Strs = new();
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PSRFTXTVersion 123.456.789-LD12"));
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PSRFTXTTOW:  0"));
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PSRFTXTWK:   1371"));
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PSRFTXTPOS:  1234567 0 0"));
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PSRFTXTCLK:  12345"));
            Strs.Add(utils.ProtocolNMEA.MakeMsg("PSRFTXTCHNL: 12"));
            Strs.Add("$BaudRate:" + baudrate.ToString() + "*02"); // Wrong CRC - that's ok here.        //Strs.Add(utils.ProtocolNMEA.MakeMsg("BaudRate:" + baudrate.ToString()));
            Strs.Add("$PowerMode:Continues*30"); // Wrong CRC - that's ok here.                         //Strs.Add(utils.ProtocolNMEA.MakeMsg("PowerMode:Continues"));
            Strs.Add("$DF PRO: NMEA, 4800, Msg:1,0,5,5,1,1,0,0,0,0*14"); // Wrong CRC - that's ok here. //Strs.Add(utils.ProtocolNMEA.MakeMsg("DF PRO: NMEA, 4800, Msg:1,0,5,5,1,1,0,0,0,0"));
            return Strs;
        }

        public List<string> MakeOutMsgSetNavi()
        {
            DateTime TimeNow = DateTime.UtcNow;
            string timeNowStr = TimeNow.ToString("HHmmss") + ".000";
            string posLatLon = "1122.3344,N,12233.4455,E";
            string speedOverGround = "52.58";
            string courseOverGround = "132.68";
            List<string> Strs = new();
            if (m_MsgsEnabled.GGA.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGGA," + timeNowStr + "," + posLatLon + ",1,12,0.7,-14.3,M,14.3,M,0.0,0000"));
            if (m_MsgsEnabled.GLL.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGLL," + posLatLon + "," + timeNowStr + ",A"));
            if (m_MsgsEnabled.GSA.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSA,A,3,07,10,13,21,16,09,17,14,08,23,09,15,1.3,0.7,1.0"));
            if (m_MsgsEnabled.GSV.IsReady())
            {
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,1,12,01,01,001,01,02,02,002,,03,03,003,,04,04,004,"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,2,12,05,05,005,05,06,06,006,,07,07,007,,08,08,008,08"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,3,12,09,09,009,09,10,10,010,,12,12,012,,13,13,013,13"));
            }
            if (m_MsgsEnabled.RMC.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPRMC," + timeNowStr + ",A," + posLatLon + "," + speedOverGround + "," + courseOverGround + "," + TimeNow.ToString("yyMMdd") + ",,,A"));
            if (m_MsgsEnabled.VTG.IsReady())
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GNVTG," + courseOverGround + ",T,,M," + speedOverGround + ",N," + Units.KnotsToKmH(speedOverGround, 2) + ",K,N"));
            // Message ZDA is not supported.
            return Strs;
        }

        public List<string> HandleMsg(string msg, int baudrate)
        {
            string[] Payload = utils.ProtocolNMEA.GetPayload(msg);
            if (Payload.Length == 0)
                return new();

            if (Payload[0] == "PSRF100" && Payload.Length == 6) // 100—SetSerialPort
            {
                int Baudrate = 0;
                try
                {
                    Baudrate = int.Parse(Payload[2]);
                }
                catch { }
            
                //int[] BRs = { 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200 }; // for the receiver
                int[] BRs = { 9600, 19200, 38400 }; // for TWR_UART
                if (Payload[1] != "1" || Payload[3] != "8" || Payload[4] != "1" || Payload[5] != "0" || Baudrate == 0 || !BRs.Contains(Baudrate))
                    throw new Exception("PSRF100, received message is not acceptable: " + msg);
            
                PortBaudrateChanged?.Invoke(this, new(Baudrate));
                return new();
            }

            if (Payload[0] == "PSRF101" && Payload.Length == 9) // 101—NavigationInitialization
            {
                if (Payload[8] == "8")
                    PortBaudrateChanged?.Invoke(this, new(9600)); // 9600 is default baudrate for this receiver.
                List<string> Strs = new();
                //Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,NMEA: ID 101 Ack Input"));
                Strs.AddRange(MakeOutMsgSetRestart(baudrate));
                return Strs;
            }

            if (Payload[0] == "PSRF103" && Payload.Length == 5) // 103—Query/Rate Control
            {
                switch (Payload[1])
                {
                    case "00": m_MsgsEnabled.GGA.SetFrequency(Payload[3]); break;
                    case "01": m_MsgsEnabled.GLL.SetFrequency(Payload[3]); break;
                    case "02": m_MsgsEnabled.GSA.SetFrequency(Payload[3]); break;
                    case "03": m_MsgsEnabled.GSV.SetFrequency(Payload[3]); break;
                    case "04": m_MsgsEnabled.RMC.SetFrequency(Payload[3]); break;
                    case "05": m_MsgsEnabled.VTG.SetFrequency(Payload[3]); break;
                    //case "08": m_MsgsEnabled.ZDA.SetFrequency(Payload[3]); break;
                    default:
                        throw new Exception("PSRF103, received message is not acceptable: " + msg);
                }
                return MakeOutMsgNoCRC("PSRFTXT,NMEA: ID 103 Ack Input");
            }
            return new();
        }
    }
}
