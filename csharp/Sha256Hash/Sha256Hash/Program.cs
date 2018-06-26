using System;
using System.Security.Cryptography; // For SHA256CryptoServiceProvider.
using System.Text; // For Encoding


namespace Sha256Hash
{
    class Program
    {
        static void Main(string[] args)
        {
            string text = "This is a text that will be hashed into a base64 string using SHA-256 !! :)";

            byte[] bytes = Encoding.UTF8.GetBytes(text);

            var sha256SvcProv = SHA256CryptoServiceProvider.Create();

            byte[] hash = sha256SvcProv.ComputeHash(bytes);

            string hashString = Convert.ToBase64String(hash);

            Console.WriteLine(text);
            Console.WriteLine(hashString);
        }
    }
}
