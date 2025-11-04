// utilsLog: 2024-08-20 (2024-10-31)
using System.Reflection;
using System.Text;

namespace utils
{
    public static class Log
    {
        private static string GetAppFileName()
        {
            return Path.GetFileNameWithoutExtension(System.Diagnostics.Process.GetCurrentProcess().MainModule!.FileName);
        }

        private static string GetLogFileName()
        {
            return GetAppFileName() + " " + DateTime.Now.ToString("yyyy-MM-dd") + ".log";
        }

        public static void RemoveOutdatedFiles()
        {
            string AppName = GetAppFileName();
            string AppPath = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location)!;
            string[] FileNames = Directory.GetFiles(AppPath, AppName + " ????-??-??.log");
            if (FileNames.Length == 0)
                return;

            Array.Sort(FileNames);

            int DatePos = AppPath.Length + 1 + AppName.Length + 1;

            DateOnly DateNow = DateOnly.FromDateTime(DateTime.Now);
            DateOnly Date;
            bool NextFileIsLeft = true;
            for (int i = FileNames.Length - 1; i >= 0; --i)
            {
                try
                {
                    string FileDate = FileNames[i].Substring(DatePos, 10); // 10 is date length

                    if (!DateOnly.TryParseExact(FileDate, "yyyy-MM-dd", out Date) || Date > DateNow)
                    {
                        File.Delete(FileNames[i]);
                        continue;
                    }

                    if (Date == DateNow)
                        continue;

                    if (NextFileIsLeft && Date < DateNow)
                    {
                        NextFileIsLeft = false;
                        continue;
                    }

                    File.Delete(FileNames[i]);
                }
                catch { }
            }
        }

        private static void WriteLine(string fileName, string value)
        {
            try
            {
                StreamWriter Stream = File.AppendText(fileName);

                string Str = "[" + DateTime.Now.ToString("HH.mm.ss") + " " + String.Format("{0:d3}", DateTime.Now.Millisecond) + "] " + value;

                Stream.WriteLine(Str);

                Stream.Close();

                Console.WriteLine(Str);
            }
            catch { }
        }

        public static void WriteTrace(string message) => WriteLine(GetLogFileName(), message);
        public static void WriteTrace(string message, byte[] value)
        {
            StringBuilder Str = new StringBuilder();

            foreach (byte ch in value)
            {
                Str.Append(string.Format("{0:X2} ", ch));
            }

            WriteLine(GetLogFileName(), message + " " + Str);
        }

        public static void WriteError(string message) => WriteLine(GetLogFileName(), message);
        public static void WriteError(Exception ex, bool shortFormat)
        {
            if (shortFormat == true)
            {
                WriteLine(GetLogFileName(), "[" + ex.Source + "] " + ex.Message);
            }
            else
            {
                WriteLine(GetLogFileName(), "[" + ex.Source + "] " + ex.Message + "\r\n" + ex.StackTrace);
            }
        }
    }
}
