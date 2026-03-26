namespace SergeM
{
    internal interface IReceiverPolicy
    {
        event EventHandler<PortBaudrateEventArgs> PortBaudrateChanged;

        List<string> MakeOutputMsgSet();
        List<string> HandleMsg(string msg, int baudrate);
    }
}
