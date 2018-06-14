using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

// http://pubs.opengroup.org/onlinepubs/9699919799/utilities/pax.html#tag_20_92_13_06

namespace tar
{
    public class TarBall
    {
        const uint MAX_NAME_SIZE = 100;
        const uint MAX_PREFIX_SIZE = 155;

        public const int LOGICAL_RECORD_SIZE = 512; // Octets

        private Stream baseStream;
        private int currentFileIndex;
        private TarHeader currentHeader;

        public TarBall(Stream stream)
        {
            if (stream == null)
            {
                throw new ArgumentNullException(nameof(stream));
            }

            baseStream = stream;
            currentHeader = null;
            currentFileIndex = -1;
        }

        public async Task AddFileAsync(string path, CancellationToken cancellationToken, bool recursive = false)
        {
            if (!File.Exists(path) && !Directory.Exists(path))
            {
                throw new FileNotFoundException(path);
            }

            FileAttributes attributes = File.GetAttributes(path);

            if (attributes.HasFlag(FileAttributes.Directory))
            {
                foreach (string childPath in Directory.GetFiles(path))
                {
                    await AddFileAsync(childPath, cancellationToken, recursive);
                }
            }
            else
            {
                TarHeader header = new TarHeader();

                header.name = Path.GetFileName(path);

                if (header.name.Length > MAX_NAME_SIZE)
                {
                    throw new ArgumentOutOfRangeException(nameof(path), $"File name exceeds maximum size supported ({MAX_NAME_SIZE} characters)");
                }

                header.prefix = Path.GetDirectoryName(path);

                if (header.prefix.Length > MAX_PREFIX_SIZE)
                {
                    throw new ArgumentOutOfRangeException(nameof(path), $"Directory path exceeds maximum size supported ({MAX_PREFIX_SIZE} characters)");
                }

                FileInfo fileInfo = new FileInfo(path);

                header.size = fileInfo.Length;

            }
        }

        private long nextHeaderPosition = 0;
        private long currentPosition = 0;

        public Task<TarHeader> MoveToNextFileAsync(CancellationToken cancellationToken)
        {
            return Task.Factory.StartNew(() =>
            {
                if (currentPosition < nextHeaderPosition)
                {
                    if (baseStream.CanSeek)
                    {
                        currentPosition = baseStream.Seek(nextHeaderPosition, SeekOrigin.Begin);
                    }
                    else
                    {
                        byte[] buffer = new byte[LOGICAL_RECORD_SIZE];
                        while (currentPosition != nextHeaderPosition)
                        {
                            
                        }
                    }
                }

                if (currentPosition != nextHeaderPosition)
                {
                    throw new Exception($"Failed reading media (unexpected stream position {currentPosition})");
                }
                else
                {
                    byte[] encodedHeader = new byte[LOGICAL_RECORD_SIZE];

                    int numBytesRead = baseStream.Read(encodedHeader, 0, LOGICAL_RECORD_SIZE);

                    if (numBytesRead != LOGICAL_RECORD_SIZE)
                    {
                        throw new InvalidDataException($"Read unexpected ammount of bytes {numBytesRead}; tarball possibly corrupted.");
                    }

                    TarHeader tarHeader = TarHeader.Parse(encodedHeader);

                    if (tarHeader == null)
                    {
                        if (isEndOfFile(encodedHeader))
                        {

                        }
                    }
                    else
                    {
                        uint size = TarHeader.GetSize(encodedHeader);

                        uint fileSizeInLogicalRecords = (511 + size) / 512;
                    }
                }

                return currentHeader;
            });
        }

        public Task<uint> GetFileCountAsync(CancellationToken cancellationToken)
        {
            uint result = 0;

            return Task.FromResult<uint>(result);
        }

        public async Task<TarHeader> GetFileDescriptionAsync(uint fileIndex)
        {
            TarHeader header = new TarHeader();


            return header;
        }

        public Stream GetFileStreamAsync(uint fileIndex)
        {
            Stream stream = null;

            return stream;
        }
    }
}
