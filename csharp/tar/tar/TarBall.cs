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

        private long currentPosition = 0;
        private long nextHeaderPosition = 0;

        private bool MoveToNextHeader()
        {
            bool result;

            if (baseStream.CanSeek)
            {
                currentPosition = baseStream.Seek(nextHeaderPosition, SeekOrigin.Begin);

                result = (currentPosition == nextHeaderPosition);
            }
            else
            {
                byte[] buffer = new byte[LOGICAL_RECORD_SIZE];

                result = true;

                while (currentPosition != nextHeaderPosition)
                {
                    Task<int> numBytesRead = baseStream.ReadAsync(buffer, 0, LOGICAL_RECORD_SIZE);
                    numBytesRead.Wait();

                    if (numBytesRead.Result != LOGICAL_RECORD_SIZE)
                    {
                        throw new InvalidDataException($"Read unexpected ammount of bytes {numBytesRead}; tarball possibly corrupted.");
                    }

                    currentPosition += numBytesRead.Result;
                }
            }

            return result;
        }

        public TarHeader GetNextHeader()
        {
            TarHeader header;

            if (currentPosition != nextHeaderPosition && !MoveToNextHeader())
            {
                header = null;
            }
            else
            {
                byte[] encodedHeader = new byte[LOGICAL_RECORD_SIZE];

                int numBytesRead = baseStream.Read(encodedHeader, 0, LOGICAL_RECORD_SIZE);

                if (numBytesRead != LOGICAL_RECORD_SIZE)
                {
                    throw new InvalidDataException($"Read unexpected ammount of bytes {numBytesRead}; tarball possibly corrupted.");
                }

                header = TarHeader.Parse(encodedHeader);

                currentHeader = header;

                currentPosition += numBytesRead;

                nextHeaderPosition += (LOGICAL_RECORD_SIZE + header.size);
            }

            return header;
        }

        public Stream GetFileStream()
        {
            Stream result;

            if (currentHeader == null)
            {
                result = null;
            }
            else
            {
                result = new TarFileReadOnlyStream(baseStream, ref currentPosition, nextHeaderPosition - 1, currentHeader.size);
            }

            return result;
        }
    }
}
