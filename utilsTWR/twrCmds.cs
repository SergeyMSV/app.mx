using System.Reflection.Metadata;
using System.Text.Json;

namespace utils.twr
{
    internal static class Cmds
    {
        public static string Make(string ep, string cmd, List<KeyValuePair<string, string>> content)
        {
            Dictionary<string, string> Data = MakeHeader(cmd, ep);
            foreach (var i in content)
            {
                Data.Add(i.Key, i.Value);
            }
            return MakeJSON(Data);
        }

        static string Make(string ep, string cmd, KeyValuePair<string, string> content)
        {
            Dictionary<string, string> Data = MakeHeader(ep, cmd);
            Data.Add(content.Key, content.Value);
            return MakeJSON(Data);
        }

        public static string MakeGetVersion()
        {
            return MakeJSON(MakeHeader("control", "version"));
        }

        public static string MakeOpen(string ep, UInt32 uartBaudrate)
        {
            if (uartBaudrate != 0)
            {
                KeyValuePair<string, string> Value = new("br", uartBaudrate.ToString());
                return Make(ep, "open", Value);
            }
            return MakeJSON(MakeHeader(ep, "open"));
        }

        public static string MakeOpen(string ep) => MakeOpen(ep, 0);

        public static string MakeClose(string ep) => MakeJSON(MakeHeader(ep, "close"));

        public static string MakeSend(string ep, string data) => Make(ep, "send", new KeyValuePair<string, string>("data", data));

        public static string MakeReceive(string ep) => MakeJSON(MakeHeader(ep, "receive"));

        //public static string Make(string ep, string cmd, Dictionary<string, string> content)
        //{
        //    content.Add("ep", ep);
        //    content.Add("cmd", cmd);
        //    return MakeJSON(content);
        //}

        public static Dictionary<string, string> MakeHeader(string ep, string cmd)
        {
            Dictionary<string, string> Data = new()
            {
                { "ep", ep },
                { "cmd", cmd },
            };
            return Data;
        }

        //static string Make(string ep, string cmd, List<KeyValuePair<string, string>> content)
        //{
        //    Dictionary<string, string> Data = MakeHeader(ep, cmd);
        //    foreach (var i in content)
        //    {
        //        Data.Add(i.Key, i.Value);
        //    }
        //    return MakeJSON(Data);
        //}
        //
        //static string Make(string ep, string cmd, KeyValuePair<string, string> content)
        //{
        //    Dictionary<string, string> Data = MakeHeader(ep, cmd);
        //    Data.Add(content.Key, content.Value);
        //    return MakeJSON(Data);
        //}

        static string MakeJSON(Dictionary<string, string> dict)
        {
            var Options = new JsonSerializerOptions
            {
                //WriteIndented = true // Each pair will be on a separate line.
            };
            return JsonSerializer.Serialize(dict, Options);
        }
    }
}
