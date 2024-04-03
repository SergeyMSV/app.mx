namespace DallasRW
{
    internal class Log
    {
        public static void WriteError(string message)
        {
            if (!Properties.Settings.Default.Log)
                return;
            utils.Log.WriteError(message);
        }

        public static void WriteTrace(string message)
        {
            if (!Properties.Settings.Default.Log)
                return;
            utils.Log.WriteTrace(message);
        }
    }
}
