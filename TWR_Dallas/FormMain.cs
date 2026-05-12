using System.Net;
using System.Text.Json;

namespace SergeM
{
    public partial class FormMain : Form
    {
        utils.twr.UDPClientEndpointDallas m_TWRClient = new();
        readonly string m_Text;
        uint m_AutoGetValuePeriod = Properties.Settings.Default.AutoGetValuePeriod;
        int m_AutoGetValuePeriodCounter = 0;
        ManualResetEvent m_WaitForResponse = new(false);

        public FormMain()
        {
            InitializeComponent();

            m_Text = Text;

            SetStateNotConnected();

            m_TWRClient.Connected += OnConnected;
            m_TWRClient.Searched += OnSearched;
            m_TWRClient.Received += OnReceived;
            Cursor = Cursors.WaitCursor;
            Task.Run(() =>
            {
                Thread.Sleep(100); // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."
                if (TWRClientOpen())
                    BeginInvoke(new Action(() => { m_TWRClient.SendSearch(); }));
                BeginInvoke(new Action(() => { Cursor = Cursors.Default; }));
            });
            AutoGetValue();
        }

        bool TWRClientOpen()
        {
            string IPAddrStr = Properties.Settings.Default.Localhost ? "127.0.0.1" : Properties.Settings.Default.IPAddressRemote;
            IPEndPoint Ep = new(IPAddress.Parse(IPAddrStr), Properties.Settings.Default.UDPPortRemote);
            return m_TWRClient.Open(Properties.Settings.Default.UDPPortLocal, Ep, Properties.Settings.Default.Log);
        }

        void OnConnected(object? sender, utils.twr.ConnectedEventArgs e)
        {
            BeginInvoke(new Action(() =>
            {
                Text = m_Text + ": " + e.Version + " (" + e.Endpoint.ToString() + ")";
            }));
        }

        void OnSearched(object? sender, utils.twr.DallasSearchedEventArgs e)
        {
            BeginInvoke(new Action(() =>
            {
                listViewBus.Items.Clear();

                if (e.ROMs.Count == 0)
                {
                    MessageBox.Show("There are no devices connected to the bus.");
                    return;
                }

                foreach (var rom in e.ROMs)
                {
                    ListViewItem item = new(rom.Value); // DisplayIndex = 0
                    //item.Tag = DateTime.Now;
                    item.SubItems.Add(rom.Key); // DisplayIndex = 1
                    item.SubItems.Add("---"); // DisplayIndex = 2
                    listViewBus.Items.Add(item);
                }

                if (!checkBoxAutoGetValue.Checked)
                    ControlsEnabled(true);
            }));
            m_WaitForResponse.Set();
        }

        void OnReceived(object? sender, utils.twr.DallasReceivedEventArgs e)
        {
            BeginInvoke(new Action(() =>
            {
                foreach (var i in listViewBus.Items)
                {
                    ((ListViewItem)i).SubItems[2].Text = "";
                }

                if (e.Values.Count == 0)
                {
                    MessageBox.Show("No measurements obtained.");
                    return;
                }

                foreach (var value in e.Values)
                {
                    foreach (var i in listViewBus.Items)
                    {
                        if (((ListViewItem)i).SubItems[1].Text == value.Key)
                        {
                            ((ListViewItem)i).SubItems[2].Text = value.Value != null ? value.Value.ToString() : "no measurement";
                        }
                    }
                }

                if (!checkBoxAutoGetValue.Checked)
                    ControlsEnabled(true);
            }));
            m_WaitForResponse.Set();
        }

        void ControlsEnabled(bool state)
        {
            Cursor = state ? Cursors.Default : Cursors.WaitCursor;
            buttonSearch.Enabled = state && !checkBoxAutoGetValue.Checked;
            buttonGetValue.Enabled = state && !checkBoxAutoGetValue.Checked;
        }

        void SetStateNotConnected()
        {
            listViewBus.Items.Clear();
            Text = m_Text + ": not connected";
            ControlsEnabled(false);
        }

        void Reconnect()
        {
            Cursor = Cursors.WaitCursor;
            SetStateNotConnected();
            m_TWRClient.Close();
            ControlsEnabled(TWRClientOpen());
            Cursor = Cursors.Default;
        }

        void GetValue()
        {
            ControlsEnabled(false);
            List<string> IDs = new();
            foreach (var i in listViewBus.Items)
            {
                if (((ListViewItem)i).SubItems[0].Text == "28") // DS18B20
                    IDs.Add(((ListViewItem)i).SubItems[1].Text);
            }
            m_WaitForResponse.Reset();
            m_TWRClient.SendGetThermo(IDs);
        }

        void AutoGetValue()
        {
            Task.Run(() =>
            {
                while (true)
                {
                    try
                    {
                        // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."
                        Thread.Sleep(1000); // [TBD] it's possible to set sleeping period twice less.

                        if (!checkBoxAutoGetValue.Checked)
                            continue;
                        if (++m_AutoGetValuePeriodCounter < m_AutoGetValuePeriod)
                            continue;
                        m_AutoGetValuePeriodCounter = 0;

                        Invoke(new Action(() => GetValue()));
                    }
                    catch (Exception ex)
                    {
                        Log.WriteError(ex.Message);
                    }
                }
            });
        }

        void buttonSettings_Click(object sender, EventArgs e)
        {
            FormSettings FormSettings = new();
            FormSettings.ShowDialog();

            m_AutoGetValuePeriodCounter = 0;
            m_AutoGetValuePeriod = Properties.Settings.Default.AutoGetValuePeriod;

            if (FormSettings.IsPortSettingsChanged)
                Reconnect();
            m_TWRClient.LogEnabled = Properties.Settings.Default.Log;
        }

        void buttonReconnect_Click(object sender, EventArgs e) => Reconnect();

        private void buttonSearch_Click(object sender, EventArgs e)
        {
            ControlsEnabled(false);
            listViewBus.Items.Clear();
            m_TWRClient.SendSearch();
        }

        private void buttonGetValue_Click(object sender, EventArgs e) => GetValue();

        private void checkBoxAutoGetValue_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxAutoGetValue.Checked)
            {
                ControlsEnabled(false);
                m_AutoGetValuePeriodCounter = 0;
            }
            else
            {
                m_WaitForResponse.WaitOne(30000); // [#] 30 s.
                ControlsEnabled(true);
            }
        }

        void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_TWRClient.Connected -= OnConnected;
            m_TWRClient.Searched -= OnSearched;
            m_TWRClient.Received -= OnReceived;
            m_TWRClient.Close();
        }

        void FormMain_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.C:
                    {
                        List<object> Items = new();
                        for (int i = 0; i < listViewBus.Items.Count; ++i)
                        {
                            Dictionary<string, string> Item = new();
                            Item.Add("family_code", listViewBus.Items[i].Text);

                            for (int si = 0; si < listViewBus.Items[i].SubItems.Count; ++si)
                            {
                                switch (si)
                                {
                                    case 1: Item.Add("id", listViewBus.Items[i].SubItems[si].Text); break;
                                    case 2: Item.Add("value", listViewBus.Items[i].SubItems[si].Text); break;
                                }
                            }
                            Items.Add(Item);
                        }
                        Dictionary<string, object> Data = new() { { "values", Items } };
                        var Options = new JsonSerializerOptions
                        {
                            //WriteIndented = true // Each pair will be on a separate line.
                        };
                        string DataJSON = JsonSerializer.Serialize(Data, Options);
                        
                        if (DataJSON.Length > 0)
                            Clipboard.SetText(DataJSON);
                        else
                            Clipboard.Clear();

                        break;
                    }
            }
        }
    }
}
