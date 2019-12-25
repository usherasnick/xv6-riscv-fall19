#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void loop(int p[2]) {
    if(fork() == 0) {
        int base, buf[1];
        if(read(p[0], buf, 4) == 0) {
            close(p[0]);
            return;
        }
        base = *buf;
        printf("prime %d\n", base);

        int pnext[2];
        pipe(pnext);
        while (read(p[0], buf, 4)) {
            if (*buf % base != 0) {
                write(pnext[1], buf, 4);
            }
        }
        close(p[0]);
        close(pnext[1]);
        loop(pnext);
        return;
    }
}

int
main(int argc, char *argv[])
{
    int p[2];
    pipe(p);
    for (int i = 2; i < 36; i++) {
        write(p[1], &i, 4);
    }
    close(p[1]);
    loop(p);
    wait();
    exit();
}