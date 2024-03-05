using System.Collections;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json.Nodes;

namespace LANTagLocator
{
    public partial class FormMain : Form
    {
        private string m_LogFileName;
        public FormMain()
        {
            InitializeComponent();

            m_LogFileName = "LANTagLocator";

            Task.Run(() => { Locator(); });
            Task.Run(() => { ListViewTagClean(); });
        }

        void Locator()
        {
            //[!] check if the main window is ready
            Thread.Sleep(100); // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."

            try
            {
                ArrayList AddrsLocal = utils.Network.GetLocalIPAddresses(Properties.Settings.Default.Network);
                if (AddrsLocal.Count != 1) // In this case only one is allowed.
                {
                    foreach (IPAddress ip in AddrsLocal) // Look for in all available networks.
                        utils.Log.WriteTrace(m_LogFileName, "IPv4: " + ip.ToString());
                    throw new Exception(string.Format("Found more than one network: {0}", AddrsLocal.Count));
                }

                IPAddress IPAddr = (IPAddress)AddrsLocal[0]!;
                IPAddress SubnetMask = utils.Network.GetSubnetMask(IPAddr);

                IPAddress IPAddrBroadcast = utils.Network.MakeBroadcast(IPAddr, SubnetMask);

                Byte[] LocatorRequest = Encoding.ASCII.GetBytes("get_tag");

                UdpClient UDPLocator = new UdpClient(Properties.Settings.Default.UDPPortLocal);
                UDPLocator.EnableBroadcast = true; // In case of true, the client can send or receive broadcast packets.
                UDPLocator.DontFragment = true;

                Task.Run(async () =>
                {
                    while (true)
                    {
                        UdpReceiveResult RecvResult = await UDPLocator.ReceiveAsync();

                        Invoke(new Action(() => ListViewDataSet(RecvResult.RemoteEndPoint, RecvResult.Buffer))); // [TBD] is it OK ?

                        //utils.Log.WriteTrace(m_LogFileName, "Receive from " + RecvResult.RemoteEndPoint.ToString(), RecvResult.Buffer);
                    }
                });

                while (true)
                {
                    UDPLocator.Send(LocatorRequest, LocatorRequest.Length, new IPEndPoint(IPAddrBroadcast, Properties.Settings.Default.UDPPortRemote));
                    //UDPLocator.SendAsync(LocatorRequest, LocatorRequest.Length, new IPEndPoint(IPAddrBroadcast, Properties.Settings.Default.UDPPort));

                    //utils.Log.WriteTrace(m_LogFileName, "Send", LocatorRequest);
                    
                    Thread.Sleep(Properties.Settings.Default.TimePeriod);
                }
            }
            catch (Exception ex)
            {
                utils.Log.WriteError(m_LogFileName, ex, true);
            }
        }

        void ListViewDataSet(IPEndPoint remoteEndPoint, byte[] recvData)
        {
            try
            {
                byte[] DataRaw = recvData.TakeWhile((byte a) => { return a != 0; }).ToArray();

                string Message = Encoding.UTF8.GetString(DataRaw);
                JsonNode Node = JsonNode.Parse(Message)!;
                JsonNode NodePlatformID = Node!["platform_id"]!;
                JsonNode NodeHostName = Node!["hostname"]!;

                string RemoteEndPointStr = remoteEndPoint.Address.ToString();

                foreach (ListViewItem i in listViewTags.Items)
                {
                    if (i.Text != RemoteEndPointStr)
                        continue;
                    for (int si = 0; si < i.SubItems.Count; ++si)
                    {
                        switch (si)
                        {
                            case 1: i.SubItems[si].Text = NodePlatformID.ToString(); break;
                            case 2: i.SubItems[si].Text = NodeHostName.ToString(); break;
                        }
                    }
                    i.Tag = DateTime.Now;
                    return;
                }

                ListViewItem item = new ListViewItem(RemoteEndPointStr); // DisplayIndex = 0
                item.Tag = DateTime.Now;

                if (NodePlatformID != null)
                    item.SubItems.Add(NodePlatformID.ToString()); // DisplayIndex = 1

                if (NodeHostName != null)
                    item.SubItems.Add(NodeHostName.ToString()); // DisplayIndex = 2

                listViewTags.Items.Add(item);
            }
            catch { }
        }

        void ListViewTagClean() // It removes expired items from listViewTags.
        {
            //[!] check if the main window is ready
            Thread.Sleep(100); // [#] It is here in order to avoid the following exception: "Invoke or BeginInvoke cannot be called on a control until the window handle has been created."

            while (true)
            {
                Invoke(new Action(() =>
                {
                    try
                    {
                        TimeSpan TimePeriodExpiration = Properties.Settings.Default.TimePeriod + new TimeSpan(0, 0, 1);
                        foreach (ListViewItem i in listViewTags.Items) // Remove expired items.
                        {
                            if (DateTime.Now - (DateTime)i.Tag > TimePeriodExpiration)
                                listViewTags.Items.Remove(i);
                        }
                    }
                    catch { }
                }));
                Thread.Sleep(500); // [#]
            }
        }
    }
}