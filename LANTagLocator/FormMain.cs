using System.Collections;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;

namespace LANTagLocator
{
    public partial class FormMain : Form
    {
        UdpClient m_UDPLocator;
        IPAddress m_IPAddrBroadcast;
        ArrayList m_ReceivedResults = new ArrayList();
        object m_ReceivedResultsLock = new object();

        public FormMain()
        {
            InitializeComponent();

            if (Properties.Settings.Default.FormMainSize.Height < this.MinimumSize.Height || Properties.Settings.Default.FormMainSize.Width < this.MinimumSize.Width)
            {
                Properties.Settings.Default.FormMainSize = this.MinimumSize;
                Properties.Settings.Default.Save();
            }

            if (this.Size.Height != Properties.Settings.Default.FormMainSize.Height || this.Size.Width != Properties.Settings.Default.FormMainSize.Width)
                this.Size = new Size(Properties.Settings.Default.FormMainSize.Width, Properties.Settings.Default.FormMainSize.Height);

            if (this.Size.Height > SystemInformation.VirtualScreen.Height || this.Size.Width > SystemInformation.VirtualScreen.Width)
                this.Size = new Size(this.MinimumSize.Width, this.MinimumSize.Height);

            timerTagsRequest.Interval = (int)Properties.Settings.Default.TimePeriod.TotalMilliseconds;
        }

        private void FormMain_Shown(object sender, EventArgs e)
        {
            InitializeLocator();

            bool LocatorEnabled = m_UDPLocator != null;

            timerTagsRequest.Enabled = LocatorEnabled;
            timerTagsListClean.Enabled = LocatorEnabled;

            RequestTags();
        }

        void InitializeLocator()
        {
            try
            {
                ArrayList AddrsLocal = utils.Network.GetLocalIPAddresses(Properties.Settings.Default.Network);
                if (AddrsLocal.Count != 1) // In this case only one is allowed.
                {
                    foreach (IPAddress ip in AddrsLocal) // Look for in all available networks.
                        utils.Log.WriteTrace("IPv4: " + ip.ToString());
                    throw new Exception(string.Format("Found more than one network: {0}", AddrsLocal.Count));
                }

                IPAddress IPAddr = (IPAddress)AddrsLocal[0]!;
                IPAddress SubnetMask = utils.Network.GetSubnetMask(IPAddr);

                m_IPAddrBroadcast = utils.Network.MakeBroadcast(IPAddr, SubnetMask);

                m_UDPLocator = new UdpClient(Properties.Settings.Default.UDPPortLocal);
                m_UDPLocator.EnableBroadcast = true; // In case of true, the client can send or receive broadcast packets.
                m_UDPLocator.DontFragment = true;

                Task.Run(async () =>
                {
                    while (true)
                    {
                        UdpReceiveResult RecvResult = await m_UDPLocator.ReceiveAsync();
                        lock (m_ReceivedResultsLock)
                        {
                            m_ReceivedResults.Add(RecvResult);
                        }
                    }
                });
            }
            catch (Exception ex)
            {
                utils.Log.WriteError(ex, true);
            }
        }

        void RequestTags()
        {
            if (m_UDPLocator == null)
                return;
            Byte[] LocatorRequest = Encoding.ASCII.GetBytes("{\"cmd\":\"get_tag\"}");
            m_UDPLocator.Send(LocatorRequest, LocatorRequest.Length, new IPEndPoint(m_IPAddrBroadcast, Properties.Settings.Default.UDPPortRemote));
        }

        void ListViewDataSet(IPEndPoint remoteEndPoint, byte[] recvData)
        {
            try
            {
                byte[] DataRaw = recvData.TakeWhile((byte a) => { return a != 0; }).ToArray();

                string Message = Encoding.UTF8.GetString(DataRaw);
                JsonNode Node = JsonNode.Parse(Message)!;
                JsonNode NodeCmd = Node["cmd"]!;

                if (NodeCmd == null || NodeCmd.ToString() != "get_tag")
                    return;

                JsonNode NodePlatformID = Node["platform_id"]!;
                JsonNode NodeHostName = Node["hostname"]!;
                JsonNode NodeUptime = Node["uptime"]!;

                string RemoteEndPointStr = remoteEndPoint.Address.ToString();

                var SetSubItem = (ListViewItem item, int index, JsonNode node) =>
                {
                    if (index < 0 || index >= item.SubItems.Count || node == null)
                        return;
                    item.SubItems[index].Text = node.ToString();
                };

                foreach (ListViewItem i in listViewTags.Items)
                {
                    if (i.Text != RemoteEndPointStr) // looking for a device string by IP-Address
                        continue;

                    SetSubItem(i, 1, NodePlatformID);
                    SetSubItem(i, 2, NodeHostName);
                    SetSubItem(i, 3, NodeUptime);

                    i.Tag = DateTime.Now;
                    return;
                }

                // if no string found for the device - one should be created

                ListViewItem item = new ListViewItem(RemoteEndPointStr); // DisplayIndex = 0
                item.SubItems.Add("-----"); // DisplayIndex = 1; PlatformID
                item.SubItems.Add("-----"); // DisplayIndex = 2; HostName
                item.SubItems.Add("-----"); // DisplayIndex = 3; Uptime

                SetSubItem(item, 1, NodePlatformID);
                SetSubItem(item, 2, NodeHostName);
                SetSubItem(item, 3, NodeUptime);

                item.Tag = DateTime.Now;

                listViewTags.Items.Add(item);
            }
            catch { }
        }

        void SendCmd(string cmdJSON)
        {
            if (m_UDPLocator == null)
                return;

            try
            {
                string IPAddr = "";
                if (listViewTags.SelectedItems.Count == 1)
                    IPAddr = listViewTags.SelectedItems[0].SubItems[0].Text; // IP-Address

                Byte[] Req = Encoding.ASCII.GetBytes(cmdJSON);
                m_UDPLocator.Send(Req, Req.Length, new IPEndPoint(IPAddress.Parse(IPAddr), Properties.Settings.Default.UDPPortRemote));

            }
            catch (Exception ex)
            {
                utils.Log.WriteError(ex, true);
            }
        }

        private void toolStripMenuItemReboot_Click(object sender, EventArgs e)
        {
            SendCmd("{\"cmd\":\"reboot\"}");
        }

        private void toolStripMenuItemHalt_Click(object sender, EventArgs e)
        {
            SendCmd("{\"cmd\":\"halt\"}");
        }

        private void FormMain_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (this.WindowState == FormWindowState.Normal &&
                (Properties.Settings.Default.FormMainSize.Height != this.Size.Height || Properties.Settings.Default.FormMainSize.Width != this.Size.Width))
            {
                Properties.Settings.Default.FormMainSize = this.Size;
                Properties.Settings.Default.Save();
            }
        }

        private void timerTagsClean_Tick(object sender, EventArgs e)
        {
            try
            {
                ArrayList RecvResults = new ArrayList();
                lock (m_ReceivedResultsLock)
                {
                    RecvResults.AddRange(m_ReceivedResults);
                    m_ReceivedResults.Clear();
                }

                foreach (UdpReceiveResult res in RecvResults)
                {
                    ListViewDataSet(res.RemoteEndPoint, res.Buffer);
                }

                TimeSpan TimePeriodExpiration = Properties.Settings.Default.TimePeriod + new TimeSpan(0, 0, 1);
                for (int i = 0; i < listViewTags.Items.Count; ++i) // Remove expired items.
                {
                    if (DateTime.Now - (DateTime)listViewTags.Items[i].Tag > TimePeriodExpiration)
                        listViewTags.Items.RemoveAt(i--);
                }
            }
            catch (Exception ex)
            {
                utils.Log.WriteError(ex, true);
            }
        }

        private void timerTagsRequest_Tick(object sender, EventArgs e)
        {
            RequestTags();
        }
    }
}