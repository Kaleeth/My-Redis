export module redis_helpers;

#include <unistd.h>

#include <cstring>
#include <iostream>

const size_t k_max_msg = 4096;

export void die(const char *msg)
{
    std::cerr << msg << " " << strerror(errno) << std::endl;
    exit(1);
}

export void msg(const char *msg)
{
    std::cerr << msg << std::endl;
}

export static void fd_set_nb(int fd)
{
	errno = 0;
	inf flags = fcntl(fd, F_GETFL, 0);
	if (errno) die("fcntl error");

	flags |= O_NONBLOCK;

	errno = 0;
	(void)fcntl(fd, F_SETFL, flags);
	if (errno) die("fcntl error 2");
}

export static int32_t read_full(int fd, char *buf, size_t n)
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

export static int32_t write_all(int fd, const char *buf, size_t n)
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

