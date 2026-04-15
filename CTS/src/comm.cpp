#include "../include/comm.h"

void set_nonblock(int fd)
{
    int fl = fcntl(fd, F_GETFL);
    if (fl > 0)
        fcntl(fd, F_SETFL, fl | O_NONBLOCK);
}