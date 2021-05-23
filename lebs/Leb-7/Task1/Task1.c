#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZEBUF 100

typedef struct
{
  char debug_mode;
  char file_name[128];
  int unit_size;
  unsigned char mem_buf[10000];
  size_t mem_count;
  char disply_hexa_mode;
} state;

struct fun_desc
{
  char *name;
  void (*fun)(state *);
};

void toggle_debug_mode(state *state)
{
  if (state->debug_mode == '1')
  {
    state->debug_mode = '0';
    printf("Debug flag now off");
  }
  else
  {
    printf("Debug flag now on");
    state->debug_mode = '1';
  }
}
void set_file_name(state *state)
{
  printf("enter a file name:\n");
  char *filename = fgets(state->file_name, SIZEBUF, stdin);
  sscanf(state->file_name, "%s", filename); //remove enter char from the scan
  if (state->debug_mode == '1')
  {
    printf("%s", state->file_name);
    printf("Debug: file name set to %s\n", state->file_name);
  }
}
void set_unit_size(state *state)
{
  printf("enter a unit size:\n");
  char buffer[SIZEBUF];
  char *sizeUnit = fgets(buffer, SIZEBUF, stdin);
  sscanf(buffer, "%s", sizeUnit);
  int size = atoi(buffer);
  if (size == 1 || size == 2 || size == 4)
  {
    state->unit_size = size;
    if (state->debug_mode == '1')
    {
      printf("Debug: set size to %d \n", state->unit_size);
    }
  }
  else
  {
    if (state->debug_mode == '1')
      printf("error invalid input size, the size unchanged.\n");
  }
}
void load_into_memory(state *state)
{
  if (strcmp(state->file_name, "") == 0)
  {
    printf("error empty file name.\n");
    return;
  }
  FILE *fp = fopen(state->file_name, "r");
  if (fp == 0)
  {
    printf("error cant open the file.\n");
    fclose(fp);
    return;
  }
  printf("enter location file.\n");
  printf("enter length file in decimal.\n");
  char bufferLoction[SIZEBUF];
  char bufferLength[SIZEBUF];
  char *loction = fgets(bufferLoction, SIZEBUF, stdin);
  sscanf(bufferLoction, "%s", loction);
  char *length = fgets(bufferLength, SIZEBUF, stdin);
  sscanf(bufferLength, "%s", length);
  if (state->debug_mode == '1')
  {
    printf("Debug: file name %s \n", state->file_name);
    printf("Debug: file loction %s \n", bufferLoction);
    printf("Debug: file length %s \n", bufferLength);
  }
  int locstionSizeDec = strtol(bufferLoction, NULL, 16);
  int lengthSize = strtol(bufferLength, NULL, 10);
  // lengthSize = lengthSize*(state->unit_size);
  fseek(fp, locstionSizeDec, SEEK_SET);
  //memcpy(state->mem_buf,fp,lengthSize);
  state->mem_count = lengthSize;
  fread(state->mem_buf, state->unit_size, lengthSize, fp);
  fclose(fp);
}

void toggle_display_mode(state *state)
{
  if (state->disply_hexa_mode == '1')
  {
    state->disply_hexa_mode = '0';
    printf("Display flag now off, decimal representation");
  }
  else
  {
    printf("Display flag now on, hexadecimal representation");
    state->disply_hexa_mode = '1';
  }
  // TODO we need to add the print in hexa
}

void memory_display(state *state)
{
  int u;
  char *end;
  char *addr;
  char bufferLength[SIZEBUF];
  printf("enter u and addr\n");
  fgets(bufferLength, SIZEBUF, stdin);
  sscanf(bufferLength, "%d" "%p",&u, &addr);
  static char *formats[] = {"%#hhx\n", "%#hx\n", "No such unit", "%#x\n"};
  end =(char*)(state->mem_buf + u * state->mem_count);
  printf ("%s",end);
  if (addr == 0)
  {
    addr = (char*)(state->mem_buf);
  }
  if (state->disply_hexa_mode == '1')
  {

    while (addr < end)
    {
      int var = *((int *)(addr));
      fprintf(stdout, formats[u - 1], var);
      addr += u;
    }
  }
  else
  {
    fwrite(addr, u, state->mem_count, stdout);
  }
}
void save_into_file(state *state)
{
  char bufferLoction[SIZEBUF];
  char bufferLength[SIZEBUF];
  char bufferAddress[SIZEBUF];
  if (strcmp(state->file_name, "") == 0)
  {
    printf("error empty file name.\n");
    return;
  }
  FILE *fp = fopen(state->file_name, "r+");
  if (fp == 0)
  {
    printf("error cant open the file.\n");
    fclose(fp);
    return;
  }
  printf("enter source memory address in hexadcimal\n");
  char *address = fgets(bufferAddress, SIZEBUF, stdin);
  sscanf(bufferAddress, "%p", &address);
  printf("enter location in hexadcimal\n");
  char *loction = fgets(bufferLoction, SIZEBUF, stdin);
  sscanf(bufferLoction, "%s", loction);
  printf("enter length\n");
  char *length = fgets(bufferLength, SIZEBUF, stdin);
  sscanf(bufferLength, "%s", length);
  int locstionSizeDec = strtol(bufferLoction, NULL, 16);
  int lengthSize = strtol(bufferLength, NULL, 10);
  fseek(fp, 0L, SEEK_END);
  int res = ftell(fp);
  fseek(fp, 0L, SEEK_SET);
  printf("%d\n",res);
  printf("%d\n",lengthSize);
  if (res <= locstionSizeDec)
  {
    printf("location size passed the file size\n");
    return;
  }
  printf("ad matai");
  fseek(fp, locstionSizeDec, SEEK_SET);
  if (address == 0)
  {
    fwrite(state->mem_buf, state->unit_size, lengthSize, fp); //using fread\fwrite so we can write the unit size as it is
  }
  else
    fwrite(address, state->unit_size, lengthSize, fp);
}

void memory_modify(state *state)
{
  char bufferLoction[SIZEBUF];
  printf("enter location in hexadcimal\n");
  char *loction = fgets(bufferLoction, SIZEBUF, stdin);
  sscanf(bufferLoction, "%s", loction);

  char bufferVal[SIZEBUF];
  printf("enter val\n");
  fgets(bufferVal, SIZEBUF, stdin);
  int val;
  sscanf(bufferVal, "%x", &val);

  int locstionSizeDec = strtol(bufferLoction, NULL, 16);
  if (state->debug_mode == '1')
  {
    printf("location : %d\n", locstionSizeDec);
    printf("val: %d\n", val);
  }
  if (sizeof(state->mem_buf) <= locstionSizeDec)
  {
    printf("location size passed the buffer size\n");
    return;
  }
  for (int i = 0; i < state->unit_size; i++)
  {
    state->mem_buf[locstionSizeDec + i] = val;
    val = val / 256;
  }
}

void quit(state *state)
{
  exit(0);
}

int main(int argc, char **argv)
{
  //   argvAtOne=argv[1];
  state *s = (state *)calloc(sizeof(state), 1);
  struct fun_desc menu[] = {{"Toggle Debug Mode", toggle_debug_mode}, {"Set File Name", set_file_name}, {"Set Unit Size", set_unit_size}, {"Load Into Memory", load_into_memory}, {"Toggle Display Mode", toggle_display_mode}, {"Memory Display", memory_display}, {"Save Into File", save_into_file}, {"Memory Modify", memory_modify}, {"Quit", quit}, {NULL, NULL}};
  while (1)
  {
    printf("Choose action: \n");
    for (int i = 0; i < sizeof(menu) / sizeof(menu[0]) - 1; i++)
      printf("%i) %s \n", i + 1, menu[i].name);
    char c;
    scanf(" %c", &c);
    printf("option:%c\n", c);
    if ((c < '0') || (c > '9'))
    {
      printf("Not within bounds \n");
      exit(0);
    }
    printf("within bounds \n");
    int num = (int)(c - 48);
    fgetc(stdin);
    menu[num - 1].fun(s);
    printf("Done\n");
  }
  return 0;
}