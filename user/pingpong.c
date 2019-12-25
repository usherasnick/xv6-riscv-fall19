#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    char buf[1];
    int parent_fd[2], child_fd[2];
    pipe(parent_fd);
    pipe(child_fd);
    if(fork() == 0) {
        read(parent_fd[0], buf, 1);
        printf("%d: received ping\n", getpid());
        close(parent_fd[0]);
        close(parent_fd[1]);

        write(child_fd[1], "b", 1);
        close(child_fd[1]);
    } else {
        write(parent_fd[1], "a", 1);
        close(parent_fd[1]);

        read(child_fd[0], buf, 1);
        printf("%d: received pong\n", getpid());
        close(child_fd[0]);
        close(child_fd[1]);
    }
    exit();
}