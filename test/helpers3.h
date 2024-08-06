#include <fcntl.h>
#include <unistd.h>

#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>

const size_t k_max_msg = 4096;

enum
{
	STATE_REQ = 0,
	STATE_RES = 1,
	STATE_END = 2, // mark the connection for deletion
};

struct Conn
{
	int fd = -1;
	uint32_t state = 0; // either STATE_REQ or STATE_RES
	// buffer for reading
	size_t rbuf_size = 0;
	uint8_t rbuf[4 + k_max_msg];
	// buffer for writing
	size_t wbuf_size = 0;
	size_t wbuf_sent = 0;
	uint8_t wbuf[4 + k_max_msg];
};

void die(const char *msg)
{
    std::cerr << msg << " " << strerror(errno) << std::endl;
    exit(1);
}

void msg(const char *msg)
{
    std::cerr << msg << std::endl;
}

static void fd_set_nb(int fd)
{
	errno = 0;
	int flags = fcntl(fd, F_GETFL, 0);
	if (errno) die("fcntl error");

	flags |= O_NONBLOCK;

	errno = 0;
	(void)fcntl(fd, F_SETFL, flags);
	if (errno) die("fcntl error 2");
}

static int32_t read_full(int fd, char *buf, size_t n)
{
	while (n > 0)
	{
		ssize_t rv = read(fd, buf, n);
		if (rv <= 0) return -1; // error, or unexpected EOF
		assert((size_t)rv <= n);
		n -= (size_t)rv; 
		buf += rv;
	}
	return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n)
{
	while (n > 0)
	{
		ssize_t rv = write(fd, buf, n);
		if (rv <= 0) return -1; // error
		assert((size_t)rv <= n);
		n -= (size_t)rv;
		buf += rv;
	}
	return 0;
}
