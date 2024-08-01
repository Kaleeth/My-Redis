#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>

#include <cstdlib>

#include "helpers2.h"


static int32_t one_request(int connfd);

int main()
{
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	std::cout << fd << std::endl;
	if (fd < 0) die("socket()");
	int val = 1;
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = ntohs(1234);
	addr.sin_addr.s_addr = ntohl(0);	// wildcard address 0.0.0.0
	
	int rv = ::bind(fd, (const sockaddr *)&addr, sizeof(addr));
	if (rv)	die("bind()");

	// listen
	rv = listen(fd, SOMAXCONN);
	if (rv)	die("listen()");

	while (true)
	{
		struct sockaddr_in client_addr = {};
		socklen_t addrlen = sizeof(client_addr);
		int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
		if (connfd < 0) continue; // error

		while (true)
		{
			int32_t err = one_request(connfd);
			if (err) break;
		}
		close(connfd);
	}
}


static int32_t one_request(int connfd)
{
	// 4 bytes header
	char rbuf[4 + k_max_msg + 1];
	errno = 0;
	int32_t err = read_full(connfd, rbuf, 4);
	if (err)
	{
		if (errno == 0) msg("EOF");
		else msg("read() error");
		return err;
	}

	uint32_t len = 0;
	memcpy(&len, rbuf, 4); // assume little endian
	if (len > k_max_msg)
	{
		msg("too long");
		return -1;
	}

	// request body
	err = read_full(connfd, &rbuf[4], len);
	if (err)
	{
		msg("read() error 2");
		return err;
	}

	// do sth
	rbuf[4 + len] = '\0';
	printf("client says: %s\n", &rbuf[4]);

	// reply using same protocol
	const char reply[] = "world";
	char wbuf[4 + sizeof(reply)];
	len = (uint32_t)strlen(reply);
	memcpy(wbuf, &len, 4);
	memcpy(&wbuf[4], reply, len);
	return write_all(connfd, wbuf, 4 + len);
}

//static void do_something(int connfd)
//{
//	char rbuf[64] = {};
//	ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
//	if (n < 0)
//	{
//		// msg("read() error");
//		die("read()");
//		return;
//	}
//	printf("client says: %s\n", rbuf);
//
//	char wbuf[] = "world";
//	write(connfd, wbuf, strlen(wbuf));
//}
