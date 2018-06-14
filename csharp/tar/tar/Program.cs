using System;
using System.IO;
using System.Threading;

namespace tar
{
    class Program
    {
        static void Main(string[] args)
        {
            FileStream stream = new FileStream("./files.tar", FileMode.Open);
            TarBall tarball = new TarBall(stream);
            tarball.MoveToFileAsync(0, CancellationToken.None).Wait();

            //tar.AddFileAsync("file1.txt").Wait();
            //tar.AddFileAsync("./file2.txt").Wait();
            //tar.AddFileAsync("./abc/file1.txt").Wait();
            //tar.AddFileAsync("./abc/").Wait();

            //int count = tar.GetFileCount(); // if basestream is cannot seek, the TarStream gets closed.
            //var fileDesc = tar.GetFileDescription(0);
            //tar.SkipToNextFile();
            //tar.GetFileStream(0);


            //TarBall tar = new TarBall(stream);
            //tar.AddFileAsync("file1.txt").Wait();
            //tar.AddFileAsync("./file2.txt").Wait();
            //tar.AddFileAsync("./abc/file1.txt").Wait();
            //tar.AddFileAsync("./abc/").Wait();
            //tar.Close();



            //// tar.Read(); reads file data out of a tar
            //// tar.CanWrite => false
            //// tar.CanSeek => false

            //byte[] buffer = new byte[1024];
            //while (tar.Read(buffer, 0, buffer.Length) != 0) // reads file contents out of a tar
            //{

            //}

            //TarBall tar = new TarBall(stream);
            //int count = tar.GetFileCount(); // if basestream is cannot seek, the TarStream gets closed.
            //var fileDesc = tar.GetFileDescription();
            //tar.SkipToNextFile();


        }
    }
}
