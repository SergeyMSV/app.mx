namespace SergeM
{
    internal class ReceiverPolicyMTK_SC872_A : IReceiverPolicy
    {
        //public event EventHandler<NaviEventArgs>? NaviChanged;
        public event EventHandler<PortBaudrateEventArgs>? PortBaudrateChanged;

        public List<string> MakeOutMsgSetRestart(int baudrate)
        {
            List<string> Strs = new();
            Strs.Add("SC872-A\r\n");
            Strs.Add("RESTART\r\n");
            Strs.Add("it has not been implemented yet.\r\n");
            return Strs;
        }

        public List<string> MakeOutMsgSetNavi()
        {
            List<string> Strs = new();
            Strs.Add("SC872-A\r\n");
            Strs.Add("it has not been implemented yet.\r\n");
            return Strs;
        }

        public List<string> HandleMsg(string msg, int baudrate)
        {
            return new();
        }
    }
}
