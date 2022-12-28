using System;
using System.Net;
using System.Text;
using System.Collections.Generic;
using System.Net.Mail;
using System.IO;
using System.Threading;
using System.Threading.Tasks;

namespace ConsoleApp
{
    class Program
    {
        static string SmtpServerAddress = "smtp.office365.com";
        static int SmtpServerPort = 587;
        static string EMailAccountName = "<your email address>"; // e.g., "someemail@outlook.com"
        static string EMailAccountPassword = "<your email password>"; // e.g., you know, your password.

        public enum PhoneCarrier
        {
            Atnt,
            TMobile,
            Verizon
        }

        public static string GetPhoneCarrierEmailDomain(PhoneCarrier phoneCarrier)
        {
            // https://www.quertime.com/article/arn-2010-11-04-1-complete-list-of-email-to-sms-gateways/

            string domain = String.Empty;

            switch(phoneCarrier)
            {
                case PhoneCarrier.Atnt:
                    domain = "txt.att.net";
                    break;
                case PhoneCarrier.TMobile:
                    domain = "tmomail.net";
                    break;
                case PhoneCarrier.Verizon:
                    domain = "vtext.com";
                    break;
                default:
                    throw new Exception("Carrier not supported");
            }

            return domain;
        }

        public static async Task SendSmsMessageAsync(PhoneCarrier phoneCarrier, string phoneNumber, string messageSubject, string message, CancellationToken cancellationToken)
        {
            MailAddress fromAddress = new MailAddress(EMailAccountName);
            MailAddress toAddress = new MailAddress($"{phoneNumber}@{GetPhoneCarrierEmailDomain(phoneCarrier)}");
            MailMessage mailMessage = new MailMessage(fromAddress, toAddress);
            mailMessage.BodyEncoding = System.Text.Encoding.UTF8;
            mailMessage.SubjectEncoding = System.Text.Encoding.UTF8;
            mailMessage.Subject = messageSubject;
            mailMessage.Body = message;

            SmtpClient smtpClient = new SmtpClient(SmtpServerAddress, SmtpServerPort);
            smtpClient.EnableSsl = true;
            smtpClient.DeliveryMethod = SmtpDeliveryMethod.Network;
            smtpClient.UseDefaultCredentials = false;
            smtpClient.Credentials = new NetworkCredential(EMailAccountName, EMailAccountPassword);
            cancellationToken.ThrowIfCancellationRequested();
            await smtpClient.SendMailAsync(mailMessage);
        }

        public static void ReadSmsMessageAsync()
        {
            // https://github.com/foens/hpop/blob/master/OpenPopExamples/Examples.cs
        }

        static async Task Main(string[] args)
        {
            using (CancellationTokenSource cts = new CancellationTokenSource(TimeSpan.FromSeconds(10)))
            {
                await SendSmsMessageAsync(
                    PhoneCarrier.TMobile, 
                    "12065550001", 
                    "Weather Alert", 
                    "It's sunny in Seattle!", 
                    cts.Token);
            }
        }
    }
}
