namespace SergeM
{
    internal class ReceiverPolicySiRF_LR9548S : IReceiverPolicy
    {
        //public event EventHandler<NaviEventArgs>? NaviChanged;
        public event EventHandler<PortBaudrateEventArgs>? PortBaudrateChanged;

        public List<string> MakeOutputMsgSet()
        {
            List<string> Strs = new();
            Strs.Add("LR9548S\r\n");
            Strs.Add("it's not suppoted.\r\n");
            return Strs;
        }

        public List<string> HandleMsg(string msg, int baudrate)
        {
            return new();
        }
    }
}
