#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int
getcmd(char *buf, int nbuf)
{
    fprintf(2, "@ ");
    memset(buf, 0, nbuf);
    gets(buf, nbuf);
    if(buf[0] == 0) // EOF
        return -1;
    return 0;
}

void
panic(char *s)
{
    fprintf(2, "%s\n", s);
    exit(-1);
}

int
fork1(void)
{
    int pid;

    pid = fork();
    if(pid == -1)
        panic("fork");
    return pid;
}

int
min(int i, int j)
{
    if(i<j)
        return i;
    else
        return j;
}

void
run(char *argv[], int count) {
    char *split_argv[10];
    int input=0, output=0, choose=0;

    for(int i=0; i<count; i++) {
        split_argv[i] = argv[i];
        if(!strcmp(argv[i], "<")) {
            close(0);
            open(argv[i+1], O_RDONLY);
            input = i;
        }
        if(!strcmp(argv[i], ">")) {
            close(1);
            open(argv[i+1], O_CREATE|O_WRONLY);
            output = i;
        }
    }

    if(input || output){
        if(input > 0 && output == 0) {
            choose = input;
        }
        if(output > 0 && input == 0){
            choose = output;
        }
        if(input > 0 && output > 0){
            choose = min(input, output);
        }
        split_argv[choose] = 0;
        exec(split_argv[0], split_argv);
        return;
    }

    exec(argv[0], argv);
    exit(0);
}

void
split_pipe(char *argv[], int i, int count) {
    char *left[10], *right[10];
    int p[2];
    int left_count, right_count;

    for(left_count=0; left_count<i; left_count++) {
        left[left_count] = argv[left_count];
    }
    left[left_count] = 0;

    for(right_count=0; right_count+i+1 < count; right_count++) {
        right[right_count] = argv[right_count+i+1];
    }
    right[right_count] = 0;

    pipe(p);
    if(fork1() == 0) {
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);
        run(right, right_count);
    } else {
        close(1);
        dup(p[1]);
        close(p[0]);
        run(left, left_count);
        wait(0);
    }
    exit(0);
}

void
runcmd(char *cmdline) {
    char *argv[20];
    char **p;
    int count = 1, cmd_length;

    cmd_length = strlen(cmdline);
    p = argv;
    *p++ = cmdline;
    for(int i=0; i<cmd_length; i++) {
        if(cmdline[i] == ' ') {
            cmdline[i] = 0;
            *p++ = cmdline+i+1;
            count++;
        }
        if(cmdline[i] == '\n') {
            cmdline[i] = 0;
        }
    }
    *p = 0;

    for(int i=0; i<count; i++) {
        if(!strcmp(argv[i], "|")) {
            split_pipe(argv, i, count);
            break;
        }
    }
    run(argv, count);
}

int main(void) {
    static char buf[100];
    int fd;

    // Ensure that three file descriptors are open.
    while((fd = open("console", O_RDWR)) >= 0){
        if(fd >= 3){
            close(fd);
            break;
        }
    }

    // Read and run input commands.
    while(getcmd(buf, sizeof(buf)) >= 0){
        if(buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' '){
            // Chdir must be called by the parent, not the child.
            buf[strlen(buf)-1] = 0;  // chop \n
            if(chdir(buf+3) < 0)
                fprintf(2, "cannot cd %s\n", buf+3);
            continue;
        }
        if(fork1() == 0)
            runcmd(buf);
        wait(0);
    }
    exit(0);
}