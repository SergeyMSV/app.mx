using System.IO.Ports;

namespace SergeM
{
    internal class ReceiverPort : IDisposable
    {
        SerialPort? m_Port;
        string m_ReceivedData = "";

        public event EventHandler<EventArgs>? Closed;
        public event EventHandler<ReceivedEventArgs>? Received;

        public bool IsOpen { get; private set; }

        public int BaudRate
        {
            get => m_Port?.BaudRate ?? 0;
            set
            {
                if (m_Port != null)
                    m_Port.BaudRate = value;
            }
        }

        public ReceiverPort(string portName, int portBaudrate)
        {
            IsOpen = false;
            try
            {
                m_Port = new SerialPort(portName, portBaudrate, Parity.None, 8, StopBits.One);
                m_Port.DataReceived += OnDataReceived;
                m_Port.Open();
                IsOpen = m_Port.IsOpen;
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
                MessageBox.Show(ex.Message);
            }
        }

        public void Dispose()
        {
            m_Port?.Close();
            m_Port?.Dispose();
        }

        public void Send(string value)
        {
            if (!IsPortAccessible())
                return;
            try
            {
                m_Port.Write(value);
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
                IsPortAccessible();
                MessageBox.Show(ex.Message); // USB Serial Converter has been disconnected
            }
        }

        bool IsPortAccessible()
        {
            if (m_Port == null)
                return false;
            if (IsOpen && IsOpen != m_Port.IsOpen)
            {
                Closed?.Invoke(this, new());
                IsOpen = m_Port.IsOpen;
            }
            return IsOpen;
        }

        void OnDataReceived(object? sender, SerialDataReceivedEventArgs e)
        {
            if (!IsPortAccessible())
                return;
            if (m_ReceivedData.Length > 100)
                m_ReceivedData = "";
            try
            {
                m_ReceivedData += m_Port.ReadExisting();
                int StrEnd = m_ReceivedData.IndexOf('\xa');
                if (StrEnd == -1)
                    return;
                StrEnd += 1; // + 1 for '\xa'
                string Str = m_ReceivedData.Substring(0, StrEnd);
                m_ReceivedData = m_ReceivedData.Substring(Str.Length);
                Received?.Invoke(this, new(Str));
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
                IsPortAccessible();
            }
        }
    }
}
