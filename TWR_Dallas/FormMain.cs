using System.Net;
using System.Text.Json;
using utils.twr;

namespace SergeM
{
    public partial class FormMain : Form
    {
        UDPClientAsyncEndpointDallas? m_TWRClient;
        readonly string m_Text;
        uint m_AutoGetValuePeriod = Properties.Settings.Default.AutoGetValuePeriod;
        int m_AutoGetValuePeriodCounter = 0;
        bool m_closingAfterDisconnect = false;
        bool m_reconnecting = false;
        System.Windows.Forms.Timer? m_AutoGetValueTimer;

        public FormMain()
        {
            InitializeComponent();

            m_Text = Text;

            Cursor = Cursors.WaitCursor;
            SetStateNotConnected();
            InitializeTWRClient();
            _ = TWRClientOpen();
            AutoGetValue();
        }

        void InitializeTWRClient()
        {
            if (m_TWRClient != null)
                m_TWRClient.Dispose();
            string IPAddrStr = Properties.Settings.Default.Localhost ? "127.0.0.1" : Properties.Settings.Default.IPAddressRemote;
            IPEndPoint Ep = new(IPAddress.Parse(IPAddrStr), Properties.Settings.Default.UDPPortRemote);
            m_TWRClient = new(Properties.Settings.Default.UDPPortLocal, Ep, Properties.Settings.Default.Log);
            m_TWRClient.Connected += OnConnected;
            m_TWRClient.Opened += OnOpened;
            m_TWRClient.Closed += OnClosed;
            m_TWRClient.Searched += OnSearched;
            m_TWRClient.Received += OnReceived;
        }

        async Task<bool> TWRClientOpen()
        {
            if (m_TWRClient == null)
                return false;
            return await m_TWRClient.Open();
        }

        void OnOpened(object? sender, EventArgs e)
        {
            Cursor = Cursors.Default;
            ControlsEnabled(true);
        }

        void OnClosed(object? sender, EventArgs e)
        {
            ControlsEnabled(false);
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
                    item.SubItems.Add(rom.Key); // DisplayIndex = 1
                    item.SubItems.Add("---"); // DisplayIndex = 2
                    listViewBus.Items.Add(item);
                }

                if (!checkBoxAutoGetValue.Checked)
                    ControlsEnabled(true);
            }));
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
        }

        void ControlsEnabled(bool state)
        {
            buttonSearch.Enabled = state && !checkBoxAutoGetValue.Checked;
            buttonGetValue.Enabled = state && !checkBoxAutoGetValue.Checked;
        }

        void SetStateNotConnected()
        {
            listViewBus.Items.Clear();
            Text = m_Text + ": not connected";
            ControlsEnabled(false);
        }

        async Task Reconnect()
        {
            if (m_reconnecting || m_closingAfterDisconnect)
                return;
            m_reconnecting = true;
            try
            {
                Cursor = Cursors.WaitCursor;
                SetStateNotConnected();
                if (m_TWRClient != null && m_TWRClient.IsOpen)
                    await m_TWRClient.Close(1000);

                InitializeTWRClient();
                if (!await TWRClientOpen())
                    Cursor = Cursors.Default;
            }
            catch (Exception ex)
            {
                Cursor = Cursors.Default;
                Log.WriteError(ex.Message);
            }
            finally
            {
                m_reconnecting = false;
            }
        }

        async Task GetValue()
        {
            if (m_TWRClient == null || !m_TWRClient.IsOpen)
                return;
            ControlsEnabled(false);
            List<string> IDs = new();
            foreach (var i in listViewBus.Items)
            {
                if (((ListViewItem)i).SubItems[0].Text == "28") // DS18B20
                    IDs.Add(((ListViewItem)i).SubItems[1].Text);
            }
            try
            {
                await m_TWRClient.SendGetThermo(IDs);
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
                ControlsEnabled(true);
            }
        }

        void AutoGetValue()
        {
            m_AutoGetValueTimer = new System.Windows.Forms.Timer { Interval = 1000 };
            m_AutoGetValueTimer.Tick += async (s, e) =>
            {
                try
                {
                    if (!checkBoxAutoGetValue.Checked)
                        return;
                    if (++m_AutoGetValuePeriodCounter < m_AutoGetValuePeriod)
                        return;
                    m_AutoGetValuePeriodCounter = 0;
                    await GetValue();
                }
                catch (Exception ex)
                {
                    Log.WriteError(ex.Message);
                }
            };
            m_AutoGetValueTimer.Start();
        }

        async void buttonSettings_Click(object sender, EventArgs e)
        {
            try
            {
                FormSettings FormSettings = new();
                FormSettings.ShowDialog();

                m_AutoGetValuePeriodCounter = 0;
                m_AutoGetValuePeriod = Properties.Settings.Default.AutoGetValuePeriod;

                if (FormSettings.IsPortSettingsChanged)
                    await Reconnect();
                if (m_TWRClient != null)
                    m_TWRClient.LogEnabled = Properties.Settings.Default.Log;
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        async void buttonReconnect_Click(object sender, EventArgs e) => await Reconnect();

        async void buttonSearch_Click(object sender, EventArgs e)
        {
            if (m_TWRClient == null)
                return;
            try
            {
                ControlsEnabled(false);
                listViewBus.Items.Clear();
                await m_TWRClient.SendSearch();
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
                ControlsEnabled(true);
            }
        }

        async void buttonGetValue_Click(object sender, EventArgs e) => await GetValue();

        private void checkBoxAutoGetValue_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxAutoGetValue.Checked)
            {
                ControlsEnabled(false);
                m_AutoGetValuePeriodCounter = 0;
            }
            else
            {
                ControlsEnabled(true);
            }
        }

        void FormMain_KeyDown(object sender, KeyEventArgs e)
        {
            if (!e.Control)
                return;

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
                        string DataJSON = JsonSerializer.Serialize(Data);

                        if (DataJSON.Length > 0)
                            Clipboard.SetText(DataJSON);
                        else
                            Clipboard.Clear();

                        break;
                    }
            }
        }

        protected override async void OnFormClosing(FormClosingEventArgs e)
        {
            base.OnFormClosing(e);
            if (m_closingAfterDisconnect)
                return;

            m_closingAfterDisconnect = true;
            m_AutoGetValueTimer?.Stop();
            if (m_TWRClient != null && m_TWRClient.IsOpen)
            {
                e.Cancel = true;
                try
                {
                    await m_TWRClient.Close(1000); // waits for "ok" up to 1 sec
                }
                catch (Exception ex)
                {
                    Log.WriteError(ex.Message);
                }
                finally
                {
                    e.Cancel = false;
                    BeginInvoke(new Action(Close));
                }
                return;
            }
        }
    }
}
