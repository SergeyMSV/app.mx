using System.IO.Ports;

namespace SergeM
{
    public partial class FormSettings : Form
    {
        public bool IsPortSettingsChanged { get; private set; }

        public FormSettings()
        {
            InitializeComponent();

            string[] Ports = SerialPort.GetPortNames();
            Array.Sort(Ports);
            comboBoxCOMPort.Items.Clear();
            comboBoxCOMPort.Items.AddRange(Ports);
            int PortIndex = Array.IndexOf(Ports, Properties.Settings.Default.COMPortNumber);
            if (PortIndex != -1)
                comboBoxCOMPort.SelectedIndex = PortIndex;

            int BaudrateIndex = comboBoxCOMBaudrate.Items.IndexOf(Properties.Settings.Default.COMPortBaudrate.ToString());
            if (BaudrateIndex != -1)
                comboBoxCOMBaudrate.SelectedIndex = BaudrateIndex;

            int ReceiverModelIndex = comboBoxReceiverModel.Items.IndexOf(Properties.Settings.Default.ReceiverModel);
            if (ReceiverModelIndex != -1)
                comboBoxReceiverModel.SelectedIndex = ReceiverModelIndex;

            checkBoxLog.Checked = Properties.Settings.Default.Log;
        }

        void buttonOK_Click(object sender, EventArgs e)
        {
            try
            {
                bool UpdateSettings = false;

                if (comboBoxCOMPort.Text != Properties.Settings.Default.COMPortNumber)
                {
                    Properties.Settings.Default.COMPortNumber = comboBoxCOMPort.Text;
                    IsPortSettingsChanged = true;
                    UpdateSettings = true;
                }

                if (comboBoxCOMBaudrate.Text != Properties.Settings.Default.COMPortBaudrate.ToString())
                {
                    Properties.Settings.Default.COMPortBaudrate = int.Parse(comboBoxCOMBaudrate.Text);
                    IsPortSettingsChanged = true;
                    UpdateSettings = true;
                }

                if (comboBoxReceiverModel.Text != Properties.Settings.Default.ReceiverModel)
                {
                    Properties.Settings.Default.ReceiverModel = comboBoxReceiverModel.Text;
                    IsPortSettingsChanged = true; // ReceiverPolicy shall be restarted too.
                    UpdateSettings = true;
                }

                if (checkBoxLog.Checked != Properties.Settings.Default.Log)
                {
                    Properties.Settings.Default.Log = checkBoxLog.Checked;
                    UpdateSettings = true;
                }

                if (UpdateSettings)
                    Properties.Settings.Default.Save();

                Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Settings");
            }
        }

        void buttonCancel_Click(object sender, EventArgs e) => Close();

        void FormSettings_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Escape)
                Close();
        }

        private void textBox_Digit_KeyPress(object sender, KeyPressEventArgs e)
        {
            e.Handled = !char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar);
        }

        private void textBoxIPAddressRemote_KeyPress(object sender, KeyPressEventArgs e)
        {
            e.Handled = !char.IsControl(e.KeyChar) && !char.IsDigit(e.KeyChar) && (e.KeyChar != '.');
        }
    }
}
