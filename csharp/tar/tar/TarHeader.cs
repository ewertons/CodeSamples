using System;
using System.Collections.Generic;
using System.Text;

namespace tar
{
    public class TarHeader
    {
        public string name;                  // 100 octets 
        public char mode;                 //   8 octets
        public char uid;                  //   8 octets
        public char gid;                  //   8 octets
        public long size;                //  12 octets
        public char mtime;               //  12 octets
        public char chksum;               //   8 octets
        public char typeflag;                //   1 octet
        public char linkname;           // 100 octets
        public string magic = "ustar";                //   6 octets
        public uint version = 0;          //   2 octets
        public char uname;               //  32 octets
        public char gname;               //  32 octets
        public char devmajor;             //   8 octets
        public char devminor;             //   8 octets
        public string prefix = null;             // 155 octets

        public static uint GetSize(byte[] encodedHeader)
        {
            if (encodedHeader == null)
            {
                throw new ArgumentNullException(nameof(encodedHeader));
            }
            else if (encodedHeader.Length != TarBall.LOGICAL_RECORD_SIZE)
            {
                throw new ArgumentException($"encodedHeader size is {encodedHeader.Length} (expected {TarBall.LOGICAL_RECORD_SIZE})");
            }

            uint result = 0;
            byte[] encodedSize = new byte[12];
            Array.Copy(encodedHeader, 124, encodedSize, 0, encodedSize.Length);

            for (int i = 10; i > 0; i--)
            {
                int digit = (encodedSize[i] - '0');

                result += (uint)(Math.Pow(10, 10 - i) * digit);
            }

            return result;
        }

        internal static TarHeader Parse(byte[] encodedHeader)
        {
            throw new NotImplementedException();
        }
    };
}
