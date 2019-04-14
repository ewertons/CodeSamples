//REFERENCES :
//IMAP PROTOCOL REFERENCE : 
//http://networking.ringofsaturn.com/Protocols/imap.php   
//http://tools.ietf.org/html/rfc1730#section-6.3.1   
//OTHER REFERENCES :  

//http://msdn.microsoft.com/en-us/library/system.net.sockets.tcpclient.aspx    -- tcp client msdn  
//http://msdn.microsoft.com/en-us/library/system.net.security.sslstream.aspx -- sslstream msdn 
//http://msdn.microsoft.com/en-us/library/ds4kkd55.aspx --  encoding.ascii.getbytes msdn  
// https://support.office.com/en-us/article/POP-IMAP-and-SMTP-settings-for-Outlook-com-d088b986-291d-42b8-9564-9c414e2aa040

using System;
using System.IO;
using System.Text; 
 
namespace imapclient
{
    class Program
    {
        static System.IO.StreamWriter sw = null;
        static System.Net.Sockets.TcpClient tcpc = null;
        static System.Net.Security.SslStream ssl = null;
        static string username, password;
        static string path;
        static int bytes = -1;
        static byte[] buffer;
        static StringBuilder sb = new StringBuilder();
        static byte[] dummy;
        static void Main(string[] args)
        {
            try
            {
                path = Environment.CurrentDirectory + "\\emailresponse.txt";

                if (System.IO.File.Exists(path))
                    System.IO.File.Delete(path);

                sw = new System.IO.StreamWriter(System.IO.File.Create(path));

                tcpc = new System.Net.Sockets.TcpClient("outlook.office365.com", 993);

                ssl = new System.Net.Security.SslStream(tcpc.GetStream());
                ssl.AuthenticateAsClient("outlook.office365.com");
                receiveResponse("");

                Console.WriteLine("username : ");
                username = Console.ReadLine();

                Console.WriteLine("password : ");
                password = Console.ReadLine();

                receiveResponse("$ LOGIN " + username + " " + password + "\r\n");

                receiveResponse("$ LIST " + "\"\"" + " \"*\"" + "\r\n");

                receiveResponse("$ SELECT INBOX\r\n");

                receiveResponse("$ STATUS INBOX (MESSAGES)\r\n");

                receiveResponse("$ SEARCH UNDELETED SINCE 1-Jan-2018 BEFORE 1-Jan-2019 FROM HomeDepotReceipt@homedepot.com\r\n");


                Console.WriteLine("enter the email number to fetch :");
                int number = int.Parse(Console.ReadLine());

                receiveResponse("$ FETCH " + number + " body[header]\r\n");
                receiveResponse("$ FETCH " + number + " body[text]\r\n");
                receiveResponse("$ FETCH " + number + " body\r\n");
                receiveResponse("$ FETCH " + number + " body[BODYSTRUCTURE]\r\n");
                string content = receiveResponse("$ FETCH " + number + " body[MULTIPART/MIXED]\r\n");

                content += receiveResponse("$ NOOP\r\n"); // workaround to read last bytes from previous command. Poor, yeah.

                int part = content.IndexOf("Content-Type: application/pdf; name=eReceipt.pdf");

                part = content.IndexOf("\r\n\r\n", part);
                string encodedContent = content.Substring(part + 4);

                //File.WriteAllText("eReceipt.pdf", Base64Decode(encodedContent));  // This results in corrupted pdf...
                File.WriteAllText("eReceipt.pdf", encodedContent); // This results in corrupted pdf too...

                //receiveResponse("$ FETCH " + number + " body[APPLICATION/OCTET-STREAM]\r\n");
                //receiveResponse("$ FETCH " + number + " body[IMAGE/GIF]\r\n");


                receiveResponse("$ LOGOUT\r\n");
            }
            catch (Exception ex)
            {
                Console.WriteLine("error: " + ex.Message);
            }
            finally
            {
                if (sw != null)
                {
                    sw.Close();
                    sw.Dispose();
                }
                if (ssl != null)
                {
                    ssl.Close();
                    ssl.Dispose();
                }
                if (tcpc != null)
                {
                    tcpc.Close();
                }
            }


            Console.ReadKey();
        }

        public static string Base64Decode(string base64EncodedData)
        {
            var base64EncodedBytes = Convert.FromBase64String(base64EncodedData);
            return Encoding.UTF8.GetString(base64EncodedBytes);
        }
        static string receiveResponse(string command)
        {
            try
            {
                if (command != "")
                {
                    if (tcpc.Connected)
                    {
                        dummy = Encoding.ASCII.GetBytes(command);
                        ssl.Write(dummy, 0, dummy.Length);
                    }
                    else
                    {
                        throw new ApplicationException("TCP CONNECTION DISCONNECTED");
                    }
                }
                ssl.Flush();

                sb.Clear();
                buffer = new byte[1024 * 1024];

                bytes = ssl.Read(buffer, 0, buffer.Length);
                sb.Append(Encoding.ASCII.GetString(new ArraySegment<byte>(buffer, 0, bytes)));

                Console.WriteLine(sb.ToString());
                sw.WriteLine(sb.ToString());
                Console.WriteLine("---------------------------------");

                return sb.ToString();
            }
            catch (Exception ex)
            {
                throw new ApplicationException(ex.Message);
            }
        }

    }
}