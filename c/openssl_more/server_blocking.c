#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>

//#include <openssl/applink.h>
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

// https://www.man7.org/linux/man-pages/man3/sockaddr.3type.html
#include <netinet/in.h>

static void InitializeSSL()
{
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();
}

static void DestroySSL()
{
    ERR_free_strings();
    EVP_cleanup();
}

static void ShutdownSSL(SSL *cSSL)
{
    SSL_shutdown(cSSL);
    SSL_free(cSSL);
}

static void print_ssl_error(SSL *cSSL, int ssl_err)
{
	int error_code = SSL_get_error(cSSL, ssl_err);

	switch(error_code)
	{
		case SSL_ERROR_ZERO_RETURN:
			printf("SSL_ERROR_ZERO_RETURN\n");
			break;
		case SSL_ERROR_WANT_READ:
			printf("SSL_ERROR_WANT_READ\n");
			break;
		case SSL_ERROR_WANT_WRITE:
			printf("SSL_ERROR_WANT_WRITE\n");
			break;

		case SSL_ERROR_WANT_CONNECT:
			printf("SSL_ERROR_WANT_CONNECT\n");
			break;
		case SSL_ERROR_WANT_ACCEPT:
			printf("SSL_ERROR_WANT_ACCEPT\n");
			break;
		case SSL_ERROR_WANT_X509_LOOKUP:
			printf("SSL_ERROR_WANT_X509_LOOKUP\n");
			break;

		case SSL_ERROR_WANT_ASYNC:
			printf("SSL_ERROR_WANT_ASYNC\n");
			break;
		case SSL_ERROR_WANT_ASYNC_JOB:
			printf("SSL_ERROR_WANT_ASYNC_JOB\n");
			break;
		case SSL_ERROR_WANT_CLIENT_HELLO_CB:
			printf("SSL_ERROR_WANT_CLIENT_HELLO_CB\n");
			break;

		case SSL_ERROR_SYSCALL:
			printf("SSL_ERROR_SYSCALL\n");
			break;
		case SSL_ERROR_SSL:
			printf("SSL_ERROR_SSL\n");
			break;				
		default:
			printf("Unknown (%d)\n", error_code);
			break;
	}
}

static char* get_errno_str(int errno_value)
{
	if (errno_value == EACCES)
	{
		return "EACCES";
	}
	else if (errno_value == EADDRINUSE)
	{
		return "EADDRINUSE";
	}
	else if (errno_value == EBADF)
	{
		return "EBADF";
	}
	else if (errno_value == EINVAL)
	{
		return "EINVAL";
	}
	else if (errno_value == ENOTSOCK)
	{
		return "ENOTSOCK";
	}
	else if (errno_value == EADDRNOTAVAIL)
	{
		return "EADDRNOTAVAIL";
	}
	else if (errno_value == EFAULT)
	{
		return "EFAULT";
	}
	else if (errno_value == ELOOP)
	{
		return "ELOOP";
	}
	else if (errno_value == ENAMETOOLONG)
	{
		return "ENAMETOOLONG";
	}
	else if (errno_value == ENOENT)
	{
		return "ENOENT";
	}
	else if (errno_value == ENOMEM)
	{
		return "ENOMEM";
	}
	else if (errno_value == ENOTDIR)
	{
		return "ENOTDIR";
	}
	else if (errno_value == EROFS)
	{
		return "EROFS";
	}
	else
	{
		return "UNKNOWN";
	}
}

static char* get_accept_errno_str(int errno_value)
{
	if (errno_value == EAGAIN)
	{
		return "EAGAIN";
	}
	else if (errno_value == EWOULDBLOCK)
	{
		return "EWOULDBLOCK";
	}
	else if (errno_value == EBADF)
	{
		return "EBADF";
	}
	else if (errno_value == ECONNABORTED)
	{
		return "ECONNABORTED";
	}
	else if (errno_value == EFAULT)
	{
		return "EFAULT";
	}
	else if (errno_value == EINTR)
	{
		return "EINTR";
	}
	else if (errno_value == EINVAL)
	{
		return "EINVAL";
	}
	else if (errno_value == EMFILE)
	{
		return "EMFILE";
	}
	else if (errno_value == ENOBUFS)
	{
		return "ENOBUFS";
	}
	else if (errno_value == ENOENT)
	{
		return "ENOENT";
	}
	else if (errno_value == ENOTSOCK)
	{
		return "ENOTSOCK";
	}
	else if (errno_value == EOPNOTSUPP)
	{
		return "EOPNOTSUPP";
	}
	else if (errno_value == EPERM)
	{
		return "EPERM";
	}
	else if (errno_value == EPROTO)
	{
		return "EPROTO";
	}
	else
	{
		return "UNKNOWN";
	}
}

void main()
{
	int sockfd, newsockfd;
	SSL_CTX *sslctx;
	SSL *cSSL;
	char ssl_errno_string[256];

    #define  serv_addr htonl(INADDR_ANY)
    #define aPortNumber 3456

	InitializeSSL();

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd< 0)
	{
		//Log and Error
		return;
	}
	struct sockaddr_in saiServerAddress;
	bzero((char *) &saiServerAddress, sizeof(saiServerAddress));
	saiServerAddress.sin_family = AF_INET;
	saiServerAddress.sin_addr.s_addr = serv_addr;
	saiServerAddress.sin_port = htons(aPortNumber);

	if (bind(sockfd, (struct sockaddr *) &saiServerAddress, sizeof(saiServerAddress)) != 0)
	{
		printf("bind failed (%s)\n", get_errno_str(errno));
		return;
	}

	struct sockaddr cli_addr;
	socklen_t clilen;

	if (listen(sockfd, SOMAXCONN) != 0)
	{
		printf("listen failed (%s)\n", get_errno_str(errno));
		return;
	}

	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

	if (newsockfd < 0)
	{
		printf("accept failed (%s)\n", get_accept_errno_str(errno));
		return;
	}

	sslctx = SSL_CTX_new(TLS_server_method());

	if (sslctx == NULL)
	{
		printf("SSL_CTX_new failed\n");
		return;
	}

	(void)SSL_CTX_set_options(sslctx, SSL_OP_SINGLE_DH_USE);

	if(SSL_CTX_use_certificate_file(sslctx, "./cert.pem" , SSL_FILETYPE_PEM) != 1)
	{
		printf("SSL_CTX_use_certificate_file failed\n");
		return;
	}

	if (SSL_CTX_use_PrivateKey_file(sslctx, "./key.pem", SSL_FILETYPE_PEM) != 1)
	{
		printf("SSL_CTX_use_PrivateKey_file failed\n");
		return;
	}

	cSSL = SSL_new(sslctx);

	if (cSSL == NULL)
	{
		unsigned long ssl_errno = ERR_get_error();
		ERR_error_string_n(ssl_errno, ssl_errno_string, sizeof(ssl_errno_string)/sizeof(ssl_errno_string[0]));
		printf("SSL_new failed (%lu: %s)\n", ssl_errno, ssl_errno_string);

		return;
	}

	if (SSL_set_fd(cSSL, newsockfd ) != 1)
	{
		unsigned long ssl_errno = ERR_get_error();
		ERR_error_string_n(ssl_errno, ssl_errno_string, sizeof(ssl_errno_string)/sizeof(ssl_errno_string[0]));
		printf("SSL_set_fd failed (%lu: %s)\n", ssl_errno, ssl_errno_string);
		return;
	}

	//Here is the SSL Accept portion.  Now all reads and writes must use SSL
	int ssl_err = SSL_accept(cSSL);
	if(ssl_err <= 0)
	{
		unsigned long ssl_errno = ERR_get_error();
		ERR_error_string_n(ssl_errno, ssl_errno_string, sizeof(ssl_errno_string)/sizeof(ssl_errno_string[0]));

		printf("SSL_accept failed (%d) - %ld:%s\n", ssl_err, ssl_errno, ssl_errno_string);

		print_ssl_error(cSSL, ssl_err);

		//Error occurred, log and close down ssl
		ShutdownSSL(cSSL);
		return;
	}

	uint8_t read_buffer[1024];
	int bytes_read;

    while ((bytes_read = SSL_read(cSSL, read_buffer, 1024)) > 0)
	{
		printf("bytes read (%d): %.*s\n", bytes_read, bytes_read, read_buffer);
		
		if ((ssl_err = SSL_write(cSSL, read_buffer, bytes_read)) <= 0)
		{
			printf("SSL_write failed\n");
			print_ssl_error(cSSL, ssl_err);
		}
	}
}
