#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/param.h"

int
main(int argc, char *argv[])
{
    if(fork() == 0) {
        char c, buf[512], *p[MAXARG];
        int cnt=0, i=0;

        while(read(0, &c, 1) == 1) {
                buf[cnt++] = c;
        }

        for(i=0; i+1<argc; i++) {
            p[i] = argv[i+1];
        }
        p[i++] = buf;
        for(int j=0; j<cnt; j++) {
            if(buf[j] == ' ' || buf[j] == '\n') {
                buf[j] = 0;
                p[i++] = buf+j+1;
            }
        }
        p[i] = 0;

        exec(argv[1], p);
    }
    wait();
    exit();

    /*if(fork() == 0) {
        char c, buf[512], **p;
        int cnt=0;

        while(read(0, &c, 1) == 1) {
            if(c == '\n') {
                buf[cnt] = 0;
                break;
            } else{
                buf[cnt++] = c;
            }
        }

        p = argv + argc;
        *p++ = buf;
        for(int j=0; j<cnt; j++) {
            if(buf[j] == ' ') {
                buf[j] = 0;
                *p++ = buf+j+1;
            }
        }
        *p = 0;

        p = argv + 1;
        while(*p) {
            printf("%s\n", *p++);
        }

        exec(argv[1], p);
    } else{
        wait();
    }
    exit();*/
}