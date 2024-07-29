// utilsNetwork: 2024-03-04
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

        public static ArrayList GetLocalIPAddresses(string network)
        {
            ArrayList AddrsRaw = GetLocalIPAddresses();
            ArrayList Addrs = new ArrayList();
            foreach (IPAddress ip in AddrsRaw)
            {
                if (Contains(network, ip))
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

        public static bool Contains(string network, IPAddress ip)
        {
            int SlashPos = network.IndexOf("/");
            if (SlashPos == -1) // We only handle network address in format "IP/PrefixLength".
                throw new NotSupportedException("Network address must include prefix length (e.g. IP/PrefixLength).");

            IPAddress NetAddr = IPAddress.Parse(network.Substring(0, SlashPos));
            if (NetAddr == null)
                throw new NotSupportedException("Wrong IP-address format.");

            if (NetAddr.AddressFamily != ip.AddressFamily) // We got something like an IPV4-Address for an IPv6-Mask. This is not valid.
                return false;

            int PrefixLengthBits = int.Parse(network.Substring(SlashPos + 1));
            if (PrefixLengthBits < 0)
                throw new NotSupportedException("Wrong prefix length format.");

            if (PrefixLengthBits == 0)
                return true;

            BitArray NetAddrBits = new BitArray(NetAddr.GetAddressBytes().ToArray());
            BitArray IpBits = new BitArray(ip.GetAddressBytes().ToArray());

            if (NetAddrBits.Length != NetAddrBits.Length)
                throw new ArgumentException("Lengths of the IP-addresses do not match.");

            for (int i = 0; i < PrefixLengthBits; ++i)
            {
                if (NetAddrBits[i] != IpBits[i])
                    return false;
            }

            return true;
        }
    }
}

