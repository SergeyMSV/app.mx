namespace SergeM
{
    class PortBaudrateEventArgs : EventArgs
    {
        public int Baudrate { get; }

        public PortBaudrateEventArgs(int baudrate)
        {
            Baudrate = baudrate;
        }
    }

    internal class ReceivedEventArgs : EventArgs
    {
        public List<string> Msgs { get; }

        public ReceivedEventArgs(List<string> msgs)
        {
            Msgs = msgs;
        }

        public ReceivedEventArgs(string msg)
        {
            Msgs = new();
            Msgs.Add(msg);
        }
    }
}
