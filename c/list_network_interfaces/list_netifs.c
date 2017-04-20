
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

int main()
{
    struct ifreq ifr;
    struct ifconf ifc;
    char buf[1024];
    int success = 0;

    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sock == -1)
    {
        printf("failed opening socket\r\n");
    }


    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;
    if (ioctl(sock, SIOCGIFCONF, &ifc) == -1)
    {
        printf("failed querying SIOCGIFCONF\r\n");
    }

    struct ifreq* it = ifc.ifc_req;
    const struct ifreq* const end = it + (ifc.ifc_len / sizeof(struct ifreq));

    for (; it != end; ++it)
    {
        strcpy(ifr.ifr_name, it->ifr_name);

        if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0)
        {
            if (! (ifr.ifr_flags & IFF_LOOPBACK)) // don't count loopback
            {
                if (ioctl(sock, SIOCGIFHWADDR, &ifr) != 0)
                {
                    printf("failed querying SIOCGIFHWADDR\r\n");
                }
                else if (ioctl(sock, SIOCGIFADDR, &ifr) != 0)
                {
                    printf("failed querying SIOCGIFADDR\r\n");
                }
                else
                {
                    struct sockaddr_in* ipaddr = (struct sockaddr_in*)&ifr.ifr_addr;
                    unsigned char* mac = (unsigned char*)ifr.ifr_hwaddr.sa_data;
                    printf("if=%s, addr=%s, macaddr=%02X:%02X:%02X:%02X:%02X:%02X\r\n", ifr.ifr_name, inet_ntoa(ipaddr->sin_addr), mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
                }
            }
        }
        else
        {
                printf("failed querying SIOCGIFFLAGS\r\n");
        }
    }

    close(sock);

    return 0;
}

