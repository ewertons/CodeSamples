using System;
using System.IO;
using System.Text;
using Microsoft.WindowsAzure.Storage;
using Microsoft.WindowsAzure.Storage.Auth;
using Microsoft.WindowsAzure.Storage.Blob;

namespace AzureBlobStorage
{
    class Program
    {
        static void Main(string[] args)
        {
            // Reference: https://stackoverflow.com/questions/40109286/accessing-azure-file-storage-from-azure-function

            string connectionString = "DefaultEndpointsProtocol=https;AccountName=MYACCOUNT;AccountKey=MYKEY;EndpointSuffix=core.windows.net";

            CloudStorageAccount storageAccount = CloudStorageAccount.Parse(connectionString);
            CloudBlobClient blobClient = storageAccount.CreateCloudBlobClient();
            CloudBlobContainer container = blobClient.GetContainerReference("mycontainer");
            CloudBlockBlob blockBlob = container.GetBlockBlobReference("myblob");

            byte[] content = Encoding.UTF8.GetBytes("bla bla bla blllllaaaaaaaa!!!");
            MemoryStream ms = new MemoryStream();
            ms.Write(content, 0, content.Length);
            ms.Seek(0, SeekOrigin.Begin);
            
            blockBlob.UploadFromStreamAsync(ms).Wait();
        }
    }
}
