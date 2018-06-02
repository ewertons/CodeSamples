using System;
using System.IO;
using System.Text;
using Microsoft.WindowsAzure.Storage;
using Microsoft.WindowsAzure.Storage.Auth;
using Microsoft.WindowsAzure.Storage.File;

namespace AzureFileStorage
{
    class Program
    {
        static void Main(string[] args)
        {
            string connectionString = "DefaultEndpointsProtocol=https;AccountName=MYACCOUNT;AccountKey=MYKEY;EndpointSuffix=core.windows.net";

            CloudStorageAccount storageAccount = CloudStorageAccount.Parse(connectionString);
            CloudFileClient fileClient = storageAccount.CreateCloudFileClient();
            CloudFileShare fileShare = fileClient.GetShareReference("myshare");
            fileShare.CreateIfNotExistsAsync().Wait();
            CloudFileDirectory dirRef = fileShare.GetRootDirectoryReference();
            CloudFile fileRef = dirRef.GetFileReference("filetest.txt");

            if (!fileRef.ExistsAsync().Result)
            {
                fileRef.CreateAsync(0).Wait();
            }

            byte[] content = Encoding.UTF8.GetBytes("bla bla bla blllllaaaaaaaa!!!");
            MemoryStream ms = new MemoryStream();
            ms.Write(content, 0, content.Length);
            ms.Seek(0, SeekOrigin.Begin);

            fileRef.UploadFromStreamAsync(ms).Wait();
        }
    }
}
