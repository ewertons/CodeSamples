using Azure.Identity;
using Azure.Security.KeyVault.Secrets;
using System;
using System.Threading.Tasks;

namespace ConsoleApp
{
    class Program
    {
        private const string KeyVaultUrl = "https://mykeyvault.vault.azure.net/";
        private const string AppTenantId = "<tenant id>";
        private const string AppClientId = "<client id>";
        private const string AppSecret = "<app secret>";
        
        private static SecretClient CreateSecretClient()
        {
            var secret = new ClientSecretCredential(
                AppTenantId,
                AppClientId,
                AppSecret);

            return new SecretClient(vaultUri: new Uri(KeyVaultUrl), credential: secret);
        }

        private static void AddSecret(SecretClient client, string name, string value, string contentType)
        {
            KeyVaultSecret secret = new KeyVaultSecret(name, value);
            secret.Properties.ContentType = contentType;
            
            client.SetSecret(secret);
        }

        private static async Task RemoveSecretAsync(SecretClient client, string name)
        {
            await client.StartDeleteSecretAsync(name);
            await client.PurgeDeletedSecretAsync(name);
        }

        private static void GetSecret(SecretClient client, string name)
        {
            KeyVaultSecret secret = client.GetSecret(name);

            Console.WriteLine("Name: " + secret.Name);
            Console.WriteLine("ContentType: " + secret.Properties.ContentType);
            Console.WriteLine("Enabled: " + secret.Properties.Enabled);
            Console.WriteLine("Value: " + secret.Value);
        }

        private static void DisableSecret(SecretClient client, string name)
        {
            KeyVaultSecret secret = client.GetSecret(name);
            secret.Properties.Enabled = false;
            client.SetSecret(secret);
        }

        private static void EnableSecret(SecretClient client, string name)
        {
            KeyVaultSecret secret = client.GetSecret(name);
            secret.Properties.Enabled = true;
            client.SetSecret(secret);
        }

        private static void UpdateSecretContentType(SecretClient client, string name, string contentType)
        {
            KeyVaultSecret secret = client.GetSecret(name);
            secret.Properties.ContentType = contentType;
            client.SetSecret(secret);
        }

        private static void ListSecrets(SecretClient client)
        {
            foreach (SecretProperties secretProperties in client.GetPropertiesOfSecrets())
            {
                Console.WriteLine("Name: " + secretProperties.Name);
                Console.WriteLine("ContentType: " + secretProperties.ContentType);
                Console.WriteLine("Enabled: " + secretProperties.Enabled);
                Console.WriteLine();
            }
        }

        static async Task Main(string[] args)
        {
            SecretClient client = CreateSecretClient();

            AddSecret(client, "MySecretName", "MySecretValue", "MySecretContentType");

            ListSecrets(client);

            UpdateSecretContentType(client, "MySecretName", "MySecretContentType2");

            GetSecret(client, "MySecretName");

            DisableSecret(client, "MySecretName");

            // Cannot do a GET on a disabled secret...
            //GetSecret(client, "MySecretName");

            //EnableSecret(client, "MySecretName");

            //GetSecret(client, "MySecretName");

            try
            {
                await RemoveSecretAsync(client, "MySecretName");
            }
            catch (Azure.RequestFailedException rfe) 
            {
                // Must enable soft-delete on KeyVault properties, otherwise you get:
                if (rfe.Status != 403) // 403 == "Forbidden"
                {
                    throw;
                }
            }
        }
    }
}
