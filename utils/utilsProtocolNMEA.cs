// utilsProtocolNMEA: 2026-03-21
using System.Reflection;
using System.Reflection.Metadata;
using System.Text;

namespace utils
{
    public static class ProtocolNMEA
    {
        public static string GetCRC(string content)
        {
            byte CRC = 0;
            for (int i = 0; i < content.Length; ++i)
            {
                CRC ^= (byte)content[i];
            }
            return CRC.ToString("X2");
        }

        public static string[] GetPayload(string msg)
        {
            if (msg.Length == 0)
                return [];
            int IndexStart = msg.IndexOf('$');
            int IndexCRC = msg.IndexOf('*');
            if (IndexStart == -1 || IndexCRC == -1)
                return [];
            int ContentStart = IndexStart + 1;
            int ContentSize = IndexCRC - ContentStart;
            string Content = msg.Substring(ContentStart, ContentSize);
            if (Content.Length == 0)
                return [];
            string CRCCalc = GetCRC(Content);
            string CRCPack = msg.Substring(IndexCRC + 1).Trim();
            if (CRCCalc != CRCPack)
                return [];
            return Content.Split(',');
        }

        public static string MakeMsg(string data) => "$" + data + "*" + GetCRC(data) + "\r\n";
        public static string MakeMsgNoCRC(string data) => "$" + data + "\r\n";
    }
}
