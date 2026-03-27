namespace SergeM
{
    public partial class FormMain : Form
    {
        Receiver m_Receiver;

        public FormMain()
        {
            InitializeComponent();
            InitializeFormSizes();
            InitializeSendRadioButtons();

            buttonStopReceive.Tag = buttonStopReceive.ForeColor;

            m_Receiver = new();
            m_Receiver.PortClosed += OnPortClosed;
            m_Receiver.PortBaudrateChanged += OnPortBaudrateChanged;
            m_Receiver.Received += OnReceived;
            SetReconnectText();
        }

        void SetReconnectText()
        {
            buttonReconnect.Text = m_Receiver.IsStarted ? "&Disconnect" : "&Connect";
            labelUARTBaudrate.Text = m_Receiver.BaudRate.ToString();
        }

        private void Restart()
        {
            m_Receiver.Restart();
            SetReconnectText();
        }

        void OnPortClosed(object? sender, EventArgs e)
        {
            if (this.IsDisposed || this.Disposing)
                return;
            Invoke(new Action(() =>
            {
                SetReconnectText();
            }));
        }

        void OnPortBaudrateChanged(object? sender, PortBaudrateEventArgs e)
        {
            if (this.IsDisposed || this.Disposing)
                return;
            Invoke(new Action(() =>
            {
                labelUARTBaudrate.Text = e.Baudrate.ToString();
            }));
        }

        void OnReceived(object? sender, ReceivedEventArgs e)
        {
            if (this.IsDisposed || this.Disposing)
                return;
            Invoke(new Action(() =>
            {
                textBoxReceive.Clear();
                foreach (string i in e.Msgs)
                {
                    textBoxReceive.Text += i;
                }
            }));
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_Receiver.PortClosed -= OnPortClosed;
            m_Receiver.PortBaudrateChanged -= OnPortBaudrateChanged;
            m_Receiver.Received -= OnReceived;
            SaveFormSizes();
            SaveSendRadioButtons();
        }

        private void buttonSettings_Click(object sender, EventArgs e)
        {
            FormSettings FormSettings = new FormSettings();
            FormSettings.ShowDialog();
            if (FormSettings.IsPortSettingsChanged && m_Receiver.IsStarted)
                Restart();
        }

        private void buttonSend_Click(object sender, EventArgs e)
        {
            if (textBoxSend.Text.Length == 0)
                return;
            if (radioButtonSendNMEA.Checked)
                m_Receiver.SendNMEA(textBoxSend.Text);
            if (radioButtonSendNMEANoCRC.Checked)
                m_Receiver.SendNMEANoCRC(textBoxSend.Text);
            if (radioButtonSendLine.Checked)
                m_Receiver.SendLine(textBoxSend.Text);
            if (radioButtonSendAsIs.Checked)
                m_Receiver.Send(textBoxSend.Text);
        }

        private void buttonReconnect_Click(object sender, EventArgs e)
        {
            if (m_Receiver.IsStarted)
            {
                m_Receiver.Stop();
            }
            else
            {
                m_Receiver.Start();
            }
            SetReconnectText();
        }

        private void buttonStopReceive_Click(object sender, EventArgs e)
        {
            if (m_Receiver.IsReceivingBlocked)
            {
                m_Receiver.IsReceivingBlocked = false;
                buttonStopReceive.Text = "Stop &Receive";
                buttonStopReceive.ForeColor = (Color)buttonStopReceive.Tag!;
            }
            else
            {
                m_Receiver.IsReceivingBlocked = true;
                buttonStopReceive.Text = "Start &Receive";
                buttonStopReceive.ForeColor = Color.Red;
            }
        }

        private void buttonSendRestart_Click(object sender, EventArgs e) => m_Receiver.SendRestartMsgs();
    }
}
