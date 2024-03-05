// utilsLog: 2024-03-03
using System.Text;

namespace utils
{
    public static class Log
    {
        private static void WriteLine(string fileName, string value)
        {
            try
            {
                StreamWriter Stream = File.AppendText(fileName);

                string Str = "[" + DateTime.Now.ToString("mm.ss") + " " + String.Format("{0:d3}", DateTime.Now.Millisecond) + "] " + value;

                Stream.WriteLine(Str);

                Stream.Close();

                Console.WriteLine(Str);
            }
            catch { }
        }

        private static string GetLogFileName(string fileName)
        {
            if (fileName == "")
                return DateTime.Now.ToString("yyyy MM dd [HH]")  + ".log";

            return DateTime.Now.ToString("yyyy MM dd [HH]") + " " + fileName + ".log";
        }

        public static void WriteTrace(string fileName, string message)
        {
            WriteLine(GetLogFileName(fileName), message);
        }

        public static void WriteTrace(string message)
        {
            WriteTrace("", message);
        }

        public static void WriteTrace(string fileName, string message, byte[] value)
        {
            StringBuilder Str = new StringBuilder();

            foreach (byte ch in value)
            {
                Str.Append(string.Format("{0:X2} ", ch));
            }

            WriteLine(GetLogFileName(fileName), message + " " + Str);
        }

        public static void WriteTrace(string message, byte[] value)
        {
            WriteTrace("", message, value);
        }

        public static void WriteError(string fileName, string message)
        {
            WriteLine(GetLogFileName(fileName), message);
        }

        public static void WriteError(string message)
        {
            WriteError("", message);
        }

        public static void WriteError(string fileName, Exception ex, bool shortFormat)
        {
            if (shortFormat == true)
            {
                WriteLine(GetLogFileName(fileName), "[" + ex.Source + "] " + ex.Message);
            }
            else
            {
                WriteLine(GetLogFileName(fileName), "[" + ex.Source + "] " + ex.Message + "\r\n" + ex.StackTrace);
            }
        }

        public static void WriteError(Exception ex, bool shortFormat)
        {
            WriteError("", ex, shortFormat);
        }
    }
}
