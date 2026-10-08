namespace SergeM
{
    internal class Log
    {
        public static void WriteError(string message)
        {
            if (Properties.Settings.Default.Log)
                utils.Log.WriteError(message);
        }

        public static void WriteError(Exception ex, bool shortFormat)
        {
            if (Properties.Settings.Default.Log)
                utils.Log.WriteError(ex, shortFormat);
        }

        public static void WriteTrace(string message)
        {
            if (Properties.Settings.Default.Log)
                utils.Log.WriteTrace(message);
        }
    }
}
