using System;
using System.Collections.Generic;
using System.Net;
using System.Net.NetworkInformation;
using System.Threading;
using System.Threading.Tasks;

namespace NetScan
{
    class Program
    {
        static async Task<string> getLocalIPAddressAsync()
        {
            string result = null;

            IPHostEntry localHostEntry = await Dns.GetHostEntryAsync(Dns.GetHostName());

            foreach (var address in localHostEntry.AddressList)
            {
                if (address.AddressFamily == System.Net.Sockets.AddressFamily.InterNetwork &&
                    !address.IsIPv4MappedToIPv6 && 
                    !address.IsIPv6LinkLocal &&
                    !address.IsIPv6Multicast &&
                    !address.IsIPv6SiteLocal &&
                    !address.IsIPv6Teredo)
                {
                    result = address.ToString();
                }
            }

            return result;
        }

        static Task<PingReply> pingAsync(string address)
        {
            Ping p = new Ping();
            return p.SendPingAsync(address);
        }

        static async Task<List<string>> discoverAssignedAddressesAsync()
        {
            List<string> result = new List<string>();

            string localHostIPAddress = await getLocalIPAddressAsync();

            string[] octets = localHostIPAddress.Split('.');

            List<Task> pings = new List<Task>();

            for (int i = 1; i < 256; i++)
            {
                pings.Add(
                    pingAsync($"{octets[0]}.{octets[1]}.{octets[2]}.{i}")
                    .ContinueWith(t => {
                        if (t.Result.Status == IPStatus.Success)
                        {
                            result.Add(t.Result.Address.ToString());
                        }
                    }));
            }

            Task.WaitAll(pings.ToArray());

            return result;
        }

        static async Task MainAsync()
        {
            List<string> addresses = await discoverAssignedAddressesAsync();

            Console.Write(String.Join(Environment.NewLine, addresses));
        }

        static void Main(string[] args)
        {
            MainAsync().Wait();
        }
    }
}
