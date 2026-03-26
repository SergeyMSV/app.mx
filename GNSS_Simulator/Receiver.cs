using System.Security.Cryptography;

namespace SergeM
{
    internal class Receiver
    {
        public event EventHandler<ReceivedEventArgs>? Received;
        public event EventHandler<PortBaudrateEventArgs>? PortBaudrateChanged;
        public event EventHandler<EventArgs>? PortClosed;

        ReceiverPort? m_Port;
        IReceiverPolicy? m_ReceiverPolicy;
        System.Threading.Timer? m_Timer;
        int m_IsTimerCallbackRunning;
        object m_PortSendingLock = new();

        public bool IsStarted { get; private set; }

        public int BaudRate { get => m_Port?.BaudRate ?? 0; }

        public Receiver()
        {
            Start();
        }

        IReceiverPolicy GetReceiverPolicy()
        {
            switch (Properties.Settings.Default.ReceiverModel)
            {
                case "EB-800A": return new ReceiverPolicyMTK_EB_800A();
                case "GSU-7x": return new ReceiverPolicySiRF_GSU_7x();
                case "SC872-A": return new ReceiverPolicyMTK_SC872_A();
                case "LR9548S": return new ReceiverPolicySiRF_LR9548S();
            }
            return new ReceiverPolicySiRF_LR9548S(); // [TBD] replace with generic one
        }

        public void Start()
        {
            if (IsStarted)
                return;
            m_ReceiverPolicy = GetReceiverPolicy();
            m_ReceiverPolicy.PortBaudrateChanged += OnPortBaudrateChanged;

            m_Port = new(Properties.Settings.Default.COMPortNumber, Properties.Settings.Default.COMPortBaudrate);
            if (!m_Port.IsOpen)
            {
                Stop();
                return;
            }
            m_Port.Closed += OnPortClosed;
            m_Port.Received += OnPortReceived;

            int Period = 1000; // ms
            m_Timer = new System.Threading.Timer(OnTimerCallback, null, Period, Period);

            IsStarted = true;
        }

        public void Stop()
        {
            IsStarted = false;

            m_Timer?.Dispose();
            if (m_Port != null)
            {
                m_Port.Received -= OnPortReceived;
                m_Port.Closed -= OnPortClosed;
                m_Port.Dispose();
            }
            if (m_ReceiverPolicy != null)
                m_ReceiverPolicy.PortBaudrateChanged -= OnPortBaudrateChanged;
        }

        public void Restart()
        {
            Stop();
            Start();
        }

        void OnPortClosed(object? sender, EventArgs e)
        {
            if (IsStarted)
                Stop();
            PortClosed?.Invoke(this, new());
        }

        void OnPortBaudrateChanged(object? sender, PortBaudrateEventArgs e)
        {
            if (m_Port == null || m_ReceiverPolicy == null || m_Timer == null)
                return;
            m_Port.BaudRate = e.Baudrate;
            PortBaudrateChanged?.Invoke(this, e);
        }

        void PortSendMsgs(List<string> msgs)
        {
            if (m_Port == null || msgs.Count == 0)
                return;
            lock (m_PortSendingLock)
            {
                foreach (string i in msgs)
                {
                    m_Port.Send(i);
                }
            }
        }

        void OnPortReceived(object? sender, ReceivedEventArgs e)
        {
            if (m_ReceiverPolicy == null)
                return;
            foreach (string i in e.Msgs)
            {
                List<string> Rsp = m_ReceiverPolicy.HandleMsg(i, BaudRate);
                PortSendMsgs(Rsp);
            }
        }

        void OnTimerCallback(object? state)
        {
            if (m_Port == null || m_ReceiverPolicy == null || m_Timer == null)
                return;
            if (Interlocked.CompareExchange(ref m_IsTimerCallbackRunning, 1, 0) == 1)
                return;
            try
            {
                List<string> Msgs = m_ReceiverPolicy.MakeOutputMsgSet();
                Received?.Invoke(this, new(Msgs));
                PortSendMsgs(Msgs);
            }
            finally
            {
                Interlocked.Exchange(ref m_IsTimerCallbackRunning, 0);
            }
        }
    }
}
