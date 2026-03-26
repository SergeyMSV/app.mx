// utilsProtocolNMEA: 2026-01-09
using System.Reflection;
using System.Text;

namespace utils
{
    public static class ProtocolNMEA
    {
        public static string GetCRC(string data)
        {
            byte checksum = 0;
            for (int i = 0; i < data.Length; ++i)
            {
                checksum ^= (byte)data[i];
            }
            return checksum.ToString("X2");
        }
    }
}
