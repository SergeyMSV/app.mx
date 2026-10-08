// utilsNetwork: 2024-10-09
using System;
using System.Collections.Generic;
using System.Net;
using System.Net.NetworkInformation;
using System.Net.Sockets;

namespace utils
{
    public static class Network
    {
        public static List<IPAddress> GetLocalIPAddresses()
        {
            List<IPAddress> Addrs = new();
            IPHostEntry HostEntry = Dns.GetHostEntry(Dns.GetHostName());
            foreach (IPAddress ip in HostEntry.AddressList)
            {
                if (ip.AddressFamily == AddressFamily.InterNetwork)
                    Addrs.Add(ip);
            }
            return Addrs;
        }

        public static List<IPAddress> GetLocalIPAddresses(string network)
        {
            List<IPAddress> AddrsRaw = GetLocalIPAddresses();
            List<IPAddress> Addrs = new();
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
            throw new ArgumentException(string.Format("Can't find subnetmask for IP address '{0}'.", address));
        }

        public static IPAddress MakeBroadcast(IPAddress address, IPAddress subnetMask)
        {
            byte[] IPBytes = address.GetAddressBytes();
            byte[] MaskBytes = subnetMask.GetAddressBytes();

            if (IPBytes.Length != MaskBytes.Length)
                throw new ArgumentException("Address and mask lengths do not match.");

            byte[] BroadcastBytes = new byte[IPBytes.Length];

            for (int i = 0; i < IPBytes.Length; ++i)
            {
                BroadcastBytes[i] = (byte)(IPBytes[i] | (~MaskBytes[i]));
            }

            return new IPAddress(BroadcastBytes);
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

            byte[] netBytes = NetAddr.GetAddressBytes();
            byte[] ipBytes = ip.GetAddressBytes();

            if (netBytes.Length != ipBytes.Length)
                throw new ArgumentException("Lengths of the IP-addresses do not match.");

            int maxPrefixBits = netBytes.Length * 8;
            if (PrefixLengthBits > maxPrefixBits)
                throw new NotSupportedException("Wrong prefix length format.");

            int fullBytes = PrefixLengthBits / 8;
            int remainderBits = PrefixLengthBits % 8;

            for (int i = 0; i < fullBytes; ++i)
            {
                if (netBytes[i] != ipBytes[i])
                    return false;
            }

            if (remainderBits == 0)
                return true;

            byte mask = (byte)(0xFF << (8 - remainderBits));
            return (netBytes[fullBytes] & mask) == (ipBytes[fullBytes] & mask);
        }
    }
}

