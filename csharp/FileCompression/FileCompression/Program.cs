using System;
using System.IO;
using System.IO.Compression;
using System.Text;

namespace FileCompression
{
    class Program
    {
        static void Main(string[] args)
        {
            string originalData = "abcdefghijklmnopqrstuvwxyz0123456789";
            byte[] outBuffer = Encoding.UTF8.GetBytes(originalData);

            const string filename = ".\\sample.gz";

            if (File.Exists(filename))
            {
                File.Delete(filename);
            }

            Stream outStream = new FileStream(filename, FileMode.CreateNew);
            GZipStream outGZipStream = new GZipStream(outStream, CompressionLevel.Optimal);

            int totalsize;
            for (totalsize = 0; totalsize < 1000000; totalsize += outBuffer.Length)
            {
                outGZipStream.Write(outBuffer, 0, outBuffer.Length);
            }

            outGZipStream.Close();
            outStream.Close();

            FileInfo fileInfo = new FileInfo(filename);

            Console.WriteLine($"Original size: {totalsize} bytes; compressed size: {fileInfo.Length} bytes ({100.0  - 100.0 * fileInfo.Length / totalsize }%)");

            Stream inStream = new FileStream(filename, FileMode.Open);
            GZipStream inGZipStream = new GZipStream(inStream, CompressionMode.Decompress);

            int bytesRead = 0;
            byte[] inBuffer = new byte[outBuffer.Length];
            bytesRead = inGZipStream.Read(inBuffer, 0, inBuffer.Length);
            // Here there are a lot more data to read, since we wrote above in a loop.
            // Getting just the first sequence for fun.

            string decompressedData = Encoding.UTF8.GetString(inBuffer);
            Console.WriteLine($"Original data: {originalData}");
            Console.WriteLine($"Decompressed data: {decompressedData}");
        }
    }
}
