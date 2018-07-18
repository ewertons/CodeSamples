using System;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;

namespace GitHubGetIssue
{
    class Program
    {
        private static async Task RunAsync()
        {
            string result;
            string issueUrl = "https://api.github.com/repos/Azure/azure-iot-sdk-c/issues/507";

            using (HttpClient httpClient = new HttpClient())
            {
                string requestUrl = issueUrl;

                HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Get, requestUrl);
                request.Headers.Add("User-Agent", "SampleApp");

                var response = await httpClient.SendAsync(request);

                result = await response.Content.ReadAsStringAsync();

                Console.WriteLine(result);
            }
        }

        static void Main(string[] args)
        {
            RunAsync().Wait();
        }
    }
}
