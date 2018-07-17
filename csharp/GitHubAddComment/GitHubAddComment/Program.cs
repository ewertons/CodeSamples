using System;
using System.Net.Http;
using System.Text;
using System.Threading.Tasks;
using Newtonsoft.Json;

namespace ConsoleApp1
{
    class Program
    {
        private static async Task RunAsync()
        {
            string UserName = "MyGithubUsername";
            string SecurityToken = "OneOfMyPersonalAccessTokens";
            string encodedAuth = Convert.ToBase64String(Encoding.UTF8.GetBytes(UserName + ":" + SecurityToken));

            string uri = "https://api.github.com/repos/MyGithubUsername/MyRepo/issues/123/comments";

            HttpRequestMessage request = new HttpRequestMessage(HttpMethod.Post, uri);
            request.Headers.Add("User-Agent", "AutoFeedbackRequester");
            request.Headers.Add("Authorization", $"Basic ${encodedAuth}");

            request.Content = new ByteArrayContent(Encoding.UTF8.GetBytes("{ \"body\": \"BLABLABLA\" }"));
            

            HttpClient client = new HttpClient();
            HttpResponseMessage response = await client.SendAsync(request);

            string content = await response.Content.ReadAsStringAsync();

            Console.WriteLine(content);
        }

        static void Main(string[] args)
        {
            RunAsync().Wait();
        }
    }
}
