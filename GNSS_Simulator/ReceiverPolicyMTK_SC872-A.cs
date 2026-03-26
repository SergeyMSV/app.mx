namespace SergeM
{
    internal class ReceiverPolicyMTK_SC872_A : IReceiverPolicy
    {
        //public event EventHandler<NaviEventArgs>? NaviChanged;
        public event EventHandler<PortBaudrateEventArgs>? PortBaudrateChanged;

        public List<string> MakeOutputMsgSet()
        {
            List<string> Strs = new();
            Strs.Add("SC872-A\r\n");
            Strs.Add("it's not suppoted.\r\n");
            return Strs;
        }

        public List<string> HandleMsg(string msg, int baudrate)
        {
            return new();
        }
    }
}
