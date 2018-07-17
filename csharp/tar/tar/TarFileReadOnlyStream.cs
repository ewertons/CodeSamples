using System;
using System.Collections.Generic;
using System.IO;
using System.Text;

namespace tar
{
    public class TarFileReadOnlyStream : Stream
    {
        private Stream baseStream;
        private long initialPosition;
        private long currentPosition;
        private long endPosition;
        private long length;

        public override bool CanRead => baseStream.CanRead;

        public override bool CanSeek => baseStream.CanRead;

        public override bool CanWrite => false;

        public override long Length => length;

        public override long Position { get => (currentPosition - initialPosition); set => throw new NotSupportedException(); }

        public TarFileReadOnlyStream(Stream baseStream, ref long currentPosition, long endPosition, long length)
        {
            this.baseStream = baseStream;
            this.initialPosition = 0;
            this.currentPosition = currentPosition;
            this.endPosition = endPosition;
            this.length = length;
        }

        public override void Flush()
        {
            throw new NotSupportedException();
        }

        public override int Read(byte[] buffer, int offset, int count)
        {
            if (count > (endPosition - currentPosition))
            {
                count = (int)(endPosition - currentPosition);
            }

            int numBytesRead = baseStream.Read(buffer, offset, count);

            currentPosition += numBytesRead;

            return numBytesRead;
        }

        public override long Seek(long offset, SeekOrigin origin)
        {
            throw new NotImplementedException();
        }

        public override void SetLength(long value)
        {
            throw new NotImplementedException();
        }

        public override void Write(byte[] buffer, int offset, int count)
        {
            throw new NotSupportedException();
        }
    }
}
