using System.Globalization;

namespace SergeM
{
    internal class MsgEnable
    {
        public int Frequency { get; set; } = 1;
        int m_Couter;

        public MsgEnable() { }
        public MsgEnable(int frequency)
        {
            Frequency = frequency;
        }

        public bool IsReady()
        {
            if (Frequency != 0 && ++m_Couter == Frequency)
            {
                m_Couter = 0;
                return true;
            }
            return false;
        }

        public void SetFrequency(string freqStr)
        {
            try
            {
                Frequency = int.Parse(freqStr);
            }
            catch
            {
                Frequency = 0;
            }
        }
    }

    internal class MsgsEnable
    {
        public MsgEnable GGA;
        public MsgEnable GLL;
        public MsgEnable GSA;
        public MsgEnable GSV;
        public MsgEnable RMC;
        public MsgEnable VTG;
        public MsgEnable ZDA;

        public MsgsEnable()
        {
            GGA = new(1);
            GLL = new(1);
            GSA = new(1);
            GSV = new(1);
            RMC = new(1);
            VTG = new(1);
            ZDA = new(1);
        }

        public MsgsEnable(int gga, int gll, int gsa, int gsv, int rmc, int vtg, int zda)
        {
            GGA = new(gga);
            GLL = new(gll);
            GSA = new(gsa);
            GSV = new(gsv);
            RMC = new(rmc);
            VTG = new(vtg);
            ZDA = new(zda);
        }
    }

    internal static class Units
    {
        public static string KnotsToKmH(string knots, byte precision)
        {
            try
            { 
                double spd = double.Parse(knots, CultureInfo.InvariantCulture);
                spd *= 1.852;
                return spd.ToString("F" + precision, CultureInfo.InvariantCulture);
            }
            catch
            {
                return "";
            }
        }
    }
}
