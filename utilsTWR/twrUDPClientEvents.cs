using System.Net;

namespace utils.twr
{
    public class ConnectedEventArgs : EventArgs
    {
        public IPEndPoint Endpoint;
        public string Version { get; }

        public ConnectedEventArgs(IPEndPoint ep, string ver)
        {
            Endpoint = ep;
            Version = ver;
        }
    }

    public class DallasSearchedEventArgs : EventArgs
    {
        public Dictionary<string, string> ROMs { get; }

        public DallasSearchedEventArgs(Dictionary<string, string> roms)
        {
            ROMs = roms;
        }
    }

    public class DallasReceivedEventArgs : EventArgs
    {
        public Dictionary<string, string> Values { get; }

        public DallasReceivedEventArgs(Dictionary<string, string> values)
        {
            Values = values;
        }
    }

    public class UARTReceivedEventArgs : EventArgs
    {
        public string Data { get; }
        public UARTReceivedEventArgs(string data)

        {
            Data = data;
        }
    }
}
