export module fd_helpers;

//export void die(const char *msg)
//{
//    std::cerr << msg << " " << strerror(errno) << std::endl;
//    exit(1);
//}
//
//export void msg(const char *msg)
//{
//    std::cerr << msg << std::endl;
//}

export static void fd_set_nb(int fd)
{
	errno = 0;
	inf flags = fcntl(fd, F_GETFL, 0);
	//if (errno) die("fcntl error");

	flags |= O_NONBLOCK;

	errno = 0;
	(void)fcntl(fd, F_SETFL, flags);
	//if (errno) die("fcntl error 2");
}

