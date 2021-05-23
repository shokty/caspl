#include "util.h"
#define APPENDandCREAT 1088
#define OPEN 5
#define SYS_WRITE 4
#define SYS_READ 3
#define STDERR 2
#define STDOUT 1
#define STDIN 0
extern int system_call(int,...);
int main(int argc, char *argv[], char *envp[])
{
  int c;
  char buf[1];
  int DbugFlag = 0;
  int writeTo = STDOUT;
  int readFrom = STDIN;
  char *fileReadFrom[1];
  char *fileWriteTo[1];
  int i;
  fileWriteTo[0] = "STDOUT";
  fileReadFrom[0] = "STDIN";
  for (i = 1; i < argc; i++)
  {
    if (argc > 1 && strcmp(argv[i], "-D") == 0)
    {
      DbugFlag = 1;
    }
    else if (argv[i][1] == 'i')
    {
      fileReadFrom[0] = argv[i] + 2;
      readFrom = system_call(OPEN, fileReadFrom[0],0, 0777);
      if(readFrom<1)
        system_call(1,0x55,5,5);
      if (DbugFlag)
      {
        system_call(SYS_WRITE, STDERR, "./", 2);
        system_call(SYS_WRITE, STDERR, fileReadFrom[0], strlen(fileReadFrom[0]));
        system_call(SYS_WRITE, STDERR, "\n", 1);
        system_call(SYS_WRITE, STDERR, "ID:5\n", 5);
        system_call(SYS_WRITE, STDERR, "RETURN:", 6);
        system_call(SYS_WRITE, STDERR, itoa(writeTo), strlen(itoa(writeTo)));
        system_call(SYS_WRITE, STDERR, "\n", 1);
      }
    }
    else if (argv[i][1] == 'o')
    {
      fileWriteTo[0] = argv[i] + 2;
      writeTo = system_call(OPEN, fileWriteTo[0], 1089, 0777);
      if (DbugFlag)
      {
        system_call(SYS_WRITE, STDERR, "./", 2);
        system_call(SYS_WRITE, STDERR, fileWriteTo[0], strlen(fileWriteTo[0]));
        system_call(SYS_WRITE, STDERR, "\n", 1);
        system_call(SYS_WRITE, STDERR, "ID:5\n", 5);
        system_call(SYS_WRITE, STDERR, "RETURN:", 7);
        system_call(SYS_WRITE, STDERR, itoa(writeTo), strlen(itoa(writeTo)));
        system_call(SYS_WRITE, STDERR, "\n", 1);
      }
    }
  }
  if (DbugFlag && writeTo == STDOUT)
  {
    system_call(SYS_WRITE, STDERR, "stdout\n", 7);
  }
  if (DbugFlag && readFrom == STDIN)
  {
    system_call(SYS_WRITE, STDERR, "stdin\n", 6);
  }
  while (strcmp("1", itoa(c = system_call(SYS_READ, readFrom, buf, 1))) == 0)
  {

    if (DbugFlag)
    {
      system_call(SYS_WRITE, STDERR, "ID:3\n", 5);
      system_call(SYS_WRITE, STDERR, "RETURN:", 7);
      system_call(SYS_WRITE, STDERR, itoa(c), 1);
      system_call(SYS_WRITE, STDOUT, "\n", 1);
    }
    if (buf[0] >= 'a' && buf[0] <= 'z')
    {
      buf[0] = buf[0] - 32;
    }
    system_call(SYS_WRITE, writeTo, buf, 1);
    if (DbugFlag && writeTo == STDOUT)
    {
      system_call(SYS_WRITE, STDOUT, "\n", 1);
    }
  }
  return 0;
}