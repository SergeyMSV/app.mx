namespace SergeM
{
    internal interface IReceiverPolicy
    {
        event EventHandler<PortBaudrateEventArgs> PortBaudrateChanged;

        List<string> MakeOutMsgSetRestart(int baudrate);
        List<string> MakeOutMsgSetNavi();
        List<string> HandleMsg(string msg, int baudrate);
    }
}
