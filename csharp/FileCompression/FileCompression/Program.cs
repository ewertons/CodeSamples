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
            byte[] buffer = Encoding.UTF8.GetBytes("abcdefghijklmnopqrstuvwxyz0123456789");

            const string filename = ".\\sample.gz";

            if (File.Exists(filename))
            {
                File.Delete(filename);
            }

            Stream stream = new FileStream(filename, FileMode.CreateNew);
            GZipStream gzipStream = new GZipStream(stream, CompressionLevel.Optimal);

            int totalsize;
            for (totalsize = 0; totalsize < 1000000; totalsize += buffer.Length)
            {
                gzipStream.Write(buffer, 0, buffer.Length);
            }

            gzipStream.Close();

            FileInfo fileInfo = new FileInfo(filename);

            Console.WriteLine($"Original size: {totalsize} bytes; compressed size: {fileInfo.Length} bytes ({100.0  - 100.0 * fileInfo.Length / totalsize }%)");
        }
    }
}
