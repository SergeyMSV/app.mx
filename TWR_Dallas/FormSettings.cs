using System.Net;

namespace SergeM
{
    public partial class FormSettings : Form
    {
        public bool IsPortSettingsChanged { get; private set; }

        public FormSettings()
        {
            InitializeComponent();

            textBoxUDPPortLocal.Text = Properties.Settings.Default.UDPPortLocal.ToString();
            textBoxUDPPortRemote.Text = Properties.Settings.Default.UDPPortRemote.ToString();
            textBoxIPAddressRemote.Text = Properties.Settings.Default.IPAddressRemote.ToString();
            textBoxAutoGetValuePeriod.Text = Properties.Settings.Default.AutoGetValuePeriod.ToString();
            checkBoxLog.Checked = Properties.Settings.Default.Log;

            checkBoxIPAddressRemoteLocalhost.Checked = Properties.Settings.Default.Localhost;
            textBoxIPAddressRemote.Enabled = !checkBoxIPAddressRemoteLocalhost.Checked;
        }

        void buttonOK_Click(object sender, EventArgs e)
        {
            try
            {
                bool UpdateSettings = false;

                if (textBoxUDPPortLocal.Text != Properties.Settings.Default.UDPPortLocal.ToString())
                {
                    Properties.Settings.Default.UDPPortLocal = ushort.Parse(textBoxUDPPortLocal.Text);
                    IsPortSettingsChanged = true;
                    UpdateSettings = true;
                }

                if (textBoxUDPPortRemote.Text != Properties.Settings.Default.UDPPortRemote.ToString())
                {
                    Properties.Settings.Default.UDPPortRemote = ushort.Parse(textBoxUDPPortRemote.Text);
                    IsPortSettingsChanged = true;
                    UpdateSettings = true;
                }

                if (checkBoxIPAddressRemoteLocalhost.Checked != Properties.Settings.Default.Localhost)
                {
                    Properties.Settings.Default.Localhost = checkBoxIPAddressRemoteLocalhost.Checked;
                    IsPortSettingsChanged = true;
                    UpdateSettings = true;
                }

                if (!Properties.Settings.Default.Localhost && textBoxIPAddressRemote.Text != Properties.Settings.Default.IPAddressRemote.ToString())
                {
                    IPAddress Addr = IPAddress.Parse(textBoxIPAddressRemote.Text);
                    if (IPAddress.IsLoopback(Addr))
                    {
                        Properties.Settings.Default.Localhost = true;
                    }
                    else
                    {
                        Properties.Settings.Default.IPAddressRemote = Addr.ToString();
                    }

                    IsPortSettingsChanged = true;
                    UpdateSettings = true;
                }

                if (textBoxAutoGetValuePeriod.Text != Properties.Settings.Default.AutoGetValuePeriod.ToString())
                {
                    uint Value = uint.Parse(textBoxAutoGetValuePeriod.Text);
                    if (Value <= 0)
                    {
                        MessageBox.Show("AutoGetValuePeriod must be greater of zero", "Settings");
                        return;
                    }
                    Properties.Settings.Default.AutoGetValuePeriod = Value;
                    UpdateSettings = true;
                }

                if (checkBoxLog.Checked != Properties.Settings.Default.Log)
                {
                    Properties.Settings.Default.Log = checkBoxLog.Checked;
                    UpdateSettings = true;
                }

                if (textBoxUDPPortLocal.Text == textBoxUDPPortRemote.Text)
                    throw new Exception("Remote and local ports must be different.");

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

        void checkBoxAPAddressRemoteLocalhost_CheckedChanged(object sender, EventArgs e)
        {
            textBoxIPAddressRemote.Enabled = !checkBoxIPAddressRemoteLocalhost.Checked;
        }

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
