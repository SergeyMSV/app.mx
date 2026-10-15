using System.Net;
using System.Text.Json;

namespace SergeM
{
    public partial class FormMain : Form
    {
        utils.twr.UDPClientEpDallas? m_TWRClient;
        readonly string m_Text;
        uint m_AutoGetValuePeriod = Properties.Settings.Default.AutoGetValuePeriod;
        int m_AutoGetValuePeriodCounter = 0;
        System.Windows.Forms.Timer? m_AutoGetValueTimer;
        bool m_StateCloseUponDisconnect = false;
        bool m_StateReconnect = false;
        volatile bool m_StateSearch = false;
        utils.MessageBox m_MessageBoxConnect = new(true);
        utils.MessageBox m_MessageBoxSearch = new(true);
        utils.MessageBox m_MessageBoxRequest = new(false);

        public FormMain()
        {
            InitializeComponent();
            m_Text = Text;
            SetStateNotConnected();
            SetControlsState(false);
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
            m_TWRClient.Failed += OnFailed;
        }

        async Task<bool> TWRClientOpen()
        {
            if (m_TWRClient == null)
                return false;
            Cursor = Cursors.WaitCursor;
            return await m_TWRClient.Open();
        }

        async void OnOpened(object? sender, EventArgs e)
        {
            try
            {
                await Search();
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
            }
        }

        void OnClosed(object? sender, EventArgs e)
        {
            SetControlsState(false);
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
                    m_MessageBoxSearch.ShowInform("There are no devices connected to the bus.");
                    SetStateRequestComplete();
                    return;
                }

                foreach (var rom in e.ROMs)
                {
                    ListViewItem item = new(rom.Value); // DisplayIndex = 0
                    item.SubItems.Add(rom.Key); // DisplayIndex = 1
                    item.SubItems.Add(""); // DisplayIndex = 2
                    listViewBus.Items.Add(item);
                }

                SetStateRequestComplete();
                m_StateSearch = false;
            }));
        }

        void ClearListViewItems()
        {
            foreach (var i in listViewBus.Items)
            {
                ((ListViewItem)i).SubItems[2].Text = "";
            }
        }

        void OnReceived(object? sender, utils.twr.DallasReceivedEventArgs e)
        {
            BeginInvoke(new Action(() =>
            {
                ClearListViewItems();

                if (e.Values.Count == 0)
                {
                    SetStateRequestFailed("No measurements obtained.");
                    return;
                }

                foreach (var value in e.Values)
                {
                    foreach (var i in listViewBus.Items)
                    {
                        if (((ListViewItem)i).SubItems[1].Text == value.Key)
                            ((ListViewItem)i).SubItems[2].Text = value.Value.ToString();
                    }
                }

                SetStateRequestComplete();
            }));
        }

        void OnFailed(object? sender, utils.twr.FailedEventArgs e)
        {
            SetStateConnectionFailed(e.Msg);
            ClearListViewItems();
        }

        void SetControlsState(bool reqState, bool errState)
        {
            bool twrOpen = m_TWRClient != null && m_TWRClient.IsOpen;
            if (!twrOpen || errState)
            {
                buttonSearch.Enabled = false;
                buttonGetValue.Enabled = false;
                checkBoxAutoGetValue.Enabled = false;
                return;
            }

            if (!checkBoxAutoGetValue.Checked)
            {
                checkBoxAutoGetValue.Enabled = reqState;
            }
            else if (reqState)
            {
                checkBoxAutoGetValue.Enabled = true;
            }
            bool en = reqState && !checkBoxAutoGetValue.Checked;
            buttonSearch.Enabled = en;
            buttonGetValue.Enabled = en;
        }
        void SetControlsStateError() => SetControlsState(false, true);
        void SetControlsState(bool state)
        {
            SetControlsState(state, false);
        }

        void SetStateNotConnected()
        {
            listViewBus.Items.Clear();
            Text = m_Text + ": not connected";
            SetControlsState(false);
        }

        void SetStateRequestComplete()
        {
            SetControlsState(true);
            Cursor = Cursors.Default;
            m_MessageBoxRequest.Clear();
        }

        void SetStateConnectionFailed(string errMsg)
        {
            SetControlsStateError();
            Cursor = Cursors.Default;
            m_MessageBoxConnect.ShowFailure(errMsg);
        }

        void SetStateRequestFailed(string errMsg)
        {
            Cursor = Cursors.Default;
            m_MessageBoxRequest.ShowFailure(errMsg);
            SetControlsState(true);
        }

        async Task Reconnect()
        {
            if (m_StateReconnect || m_StateCloseUponDisconnect)
                return;
            m_StateReconnect = true;
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
                m_StateReconnect = false;
            }
        }

        async Task Search()
        {
            if (m_TWRClient == null)
                return;
            try
            {
                m_StateSearch = true;
                Cursor = Cursors.WaitCursor;
                SetControlsState(false);
                listViewBus.Items.Clear();
                await m_TWRClient.SendSearch();
            }
            catch (Exception ex)
            {
                Log.WriteError(ex.Message);
                SetControlsState(true);
                Cursor = Cursors.Default;
                m_StateSearch = false;
            }
        }

        async Task GetValue()
        {
            if (m_TWRClient == null || !m_TWRClient.IsOpen)
                return;
            SetControlsState(false);
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
                SetControlsState(true);
            }
        }

        void AutoGetValue()
        {
            m_AutoGetValueTimer = new System.Windows.Forms.Timer { Interval = 1000 };
            m_AutoGetValueTimer.Tick += async (s, e) =>
            {
                try
                {
                    if (!checkBoxAutoGetValue.Checked || m_StateSearch)// || !m_OperationInProgress) // !checkBoxAutoGetValue.Enabled)
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

        async void buttonSearch_Click(object sender, EventArgs e) => await Search();

        async void buttonGetValue_Click(object sender, EventArgs e)
        {
            Cursor = Cursors.WaitCursor;
            m_MessageBoxRequest.Clear();
            await GetValue();
        }

        private void checkBoxAutoGetValue_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBoxAutoGetValue.Checked)
                m_AutoGetValuePeriodCounter = 0;
            SetControlsState(!checkBoxAutoGetValue.Checked);
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
            if (m_StateCloseUponDisconnect)
                return;

            m_StateCloseUponDisconnect = true;
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
