// 2024-03-04
using System.Collections;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;

namespace utils
{
    public static class Network
    {
        public static ArrayList GetLocalIPAddresses()
        {
            ArrayList Addrs = new ArrayList();
            IPHostEntry HostEntry = Dns.GetHostEntry(Dns.GetHostName());
            foreach (IPAddress ip in HostEntry.AddressList)
            {
                if (ip.AddressFamily == AddressFamily.InterNetwork)
                    Addrs.Add(ip);
            }
            return Addrs;
        }

        public static ArrayList GetLocalIPAddresses(IPAddress networkAddress)
        {
            ArrayList AddrsRaw = GetLocalIPAddresses();
            ArrayList Addrs = new ArrayList();
            foreach (IPAddress ip in AddrsRaw)
            {
                if (Contains(networkAddress, ip))
                    Addrs.Add(ip);
            }
            return Addrs;
        }

        public static IPAddress GetSubnetMask(IPAddress address)
        {
            foreach (NetworkInterface adapter in NetworkInterface.GetAllNetworkInterfaces())
            {
                foreach (UnicastIPAddressInformation unicastIPAddressInformation in adapter.GetIPProperties().UnicastAddresses)
                {
                    if (unicastIPAddressInformation.Address.AddressFamily == AddressFamily.InterNetwork)
                    {
                        if (address.Equals(unicastIPAddressInformation.Address))
                        {
                            return unicastIPAddressInformation.IPv4Mask;
                        }
                    }
                }
            }
            throw new ArgumentException(string.Format("Can't find subnetmask for IP address '{0}'", address));
        }

        public static IPAddress MakeBroadcast(IPAddress address, IPAddress subnetMask)
        {
            //byte[] Addr = address.GetAddressBytes();
            //byte[] Mask = subnetMask.GetAddressBytes();

            //if (Addr.Length != Mask.Length)
            //    throw new ArgumentException(string.Format("The Address and Mask are of different sizes.", address));

            //byte[] AddrBroad = new byte[Mask.Length];
            //for (int i = 0; i < Addr.Length; ++i)
            //{
            //    AddrBroad[i] = (byte)(Addr[i] | Mask[i] ^ 0xFF);
            //}
            //return IPAddress.Parse((AddrBroad);
            IPAddress Addr = address;
            Addr.Address = Addr.Address | (subnetMask.Address ^ IPAddress.Broadcast.Address);
            return Addr;
        }

        public static bool Contains(IPAddress network, IPAddress host)
        {
            byte[] AddrNetwork = network.GetAddressBytes();
            byte[] AddrHost = host.GetAddressBytes();

            if (AddrNetwork.Length != AddrHost.Length)
                return false;

            int AddrNetworkPrefix = 0; // It's not the same as Subnet Mask.
            for (int i = 0; i < AddrNetwork.Length; ++i)
            {
                if (AddrNetwork[i] != 0)
                    AddrNetworkPrefix = i + 1;
            }

            for (int i = 0; i < AddrNetworkPrefix; ++i)
            {
                if (AddrNetwork[i] != AddrHost[i])
                    return false;
            }

            return true;
        }
    }
}

