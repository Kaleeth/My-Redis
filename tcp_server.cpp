#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <cstring>
#include <cstdlib>
#include <iostream>

void die(const char *msg)
{
    std::cerr << msg << " " << strerror(errno) << std::endl;
    exit(1);
}
static void do_something(int connf);

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

		do_something(connfd);
		close(connfd);
	}
}

static void do_something(int connfd)
{
	char rbuf[64] = {};
	ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
	if (n < 0)
	{
		// msg("read() error");
		die("read()");
		return;
	}
	printf("client says: %s\n", rbuf);

	char wbuf[] = "world";
	write(connfd, wbuf, strlen(wbuf));
}
