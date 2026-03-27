namespace SergeM
{
    internal class ReceiverPolicySiRF_GSU_7x : IReceiverPolicy
    {
        bool m_MsgGGA_Enabled = true;
        bool m_MsgGLL_Enabled = true;
        bool m_MsgGSA_Enabled = true;
        bool m_MsgGSV_Enabled = true;
        bool m_MsgRMC_Enabled = true;
        bool m_MsgVTG_Enabled = true;
        bool m_MsgZDA_Enabled = true;

        //public event EventHandler<NaviEventArgs>? NaviChanged;
        public event EventHandler<PortBaudrateEventArgs>? PortBaudrateChanged;

        public List<string> MakeOutMsgSetRestart(int baudrate)
        {
            List<string> Strs = new();
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,NMEA: ID 101 Ack Input"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,GSU-7x : Position Co.,Ltd.2009"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,Firmware Checksum: 1ac8 "));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,TOW:  0"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,WK:   1234"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,POS:  1234567 0 0"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,CLK:  12345"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,CHNL: 12"));
            Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,Baud rate: " + baudrate.ToString()));
            return Strs;
        }

        public List<string> MakeOutMsgSetNavi()
        {
            DateTime TimeNow = DateTime.UtcNow;
            List<string> Strs = new();
            if (m_MsgGGA_Enabled)
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGGA," + TimeNow.ToString("HHmmss") + ".000,1122.3344,N,12233.4455,E,1,12,00.7,00639.3,M,0014.3,M,000.0,0000"));
            if (m_MsgGLL_Enabled)
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGLL,3600.0000,N,13600.0000,E,000124.168,V"));
            if (m_MsgGSA_Enabled)
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSA,A,3,07,10,13,21,16,09,17,14,08,23,09,15,01.3,00.7,01.0"));
            if (m_MsgGSV_Enabled)
            {
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,1,12,01,01,001,01,02,02,002,,03,03,003,,04,04,004,"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,2,12,05,05,005,05,06,06,006,,07,07,007,,08,08,008,08"));
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPGSV,3,3,12,09,09,009,09,10,10,010,,12,12,012,,13,13,013,13"));
            }
            if (m_MsgRMC_Enabled)
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPRMC," + TimeNow.ToString("HHmmss") + ".000,V,1122.3344,N,12233.4455,E,9999.99,999.99," + TimeNow.ToString("yyMMdd") + ",,"));
            if (m_MsgVTG_Enabled)
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPVTG,999.99,T,,M,9999.99,N,9999.99,K"));
            if (m_MsgZDA_Enabled)
                Strs.Add(utils.ProtocolNMEA.MakeMsg("GPZDA," + TimeNow.ToString("HHmmss") + ".000," + TimeNow.ToString("dd") + "," + TimeNow.ToString("MM") + "," + TimeNow.ToString("yyyy") + ",,"));
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
                return MakeOutMsgSetRestart(baudrate);
            }

            if (Payload[0] == "PSRF103" && Payload.Length == 5) // 103—Query/Rate Control
            {
                switch (Payload[1])
                {
                    case "00": m_MsgGGA_Enabled = Payload[3] == "00" ? false : true; break;
                    case "01": m_MsgGLL_Enabled = Payload[3] == "00" ? false : true; break;
                    case "02": m_MsgGSA_Enabled = Payload[3] == "00" ? false : true; break;
                    case "03": m_MsgGSV_Enabled = Payload[3] == "00" ? false : true; break;
                    case "04": m_MsgRMC_Enabled = Payload[3] == "00" ? false : true; break;
                    case "05": m_MsgVTG_Enabled = Payload[3] == "00" ? false : true; break;
                    case "08": m_MsgZDA_Enabled = Payload[3] == "00" ? false : true; break;
                    default:
                        throw new Exception("PSRF103, received message is not acceptable: " + msg);
                }
                List<string> Strs = new();
                Strs.Add(utils.ProtocolNMEA.MakeMsgNoCRC("PSRFTXT,NMEA: ID 103 Ack Input"));
                return Strs;
            }
            return new();
        }
    }
}
