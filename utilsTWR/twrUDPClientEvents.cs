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

    public class UARTReceivedEventArgs : EventArgs
    {
        public string Data { get; }
        public UARTReceivedEventArgs(string data)

        {
            Data = data;
        }
    }
}
