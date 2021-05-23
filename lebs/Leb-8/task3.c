#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "elf.h"
int fd = -1;
int debug_mode = 0;
struct fun_desc
{
    char *name;
    void (*fun)();
};

void toggle_debug_mode()
{
    if (debug_mode == '1')
    {
        debug_mode = '0';
        printf("Debug flag now off");
    }
    else
    {
        printf("Debug flag now on");
        debug_mode = '1';
    }
}
void examine_ELF_file()
{
    void *map_start;     /* will point to the start of the memory mapped file */
    struct stat fd_stat; /* this is needed to  the size of the file */
    Elf32_Ehdr *header;  /* this will point to the header structure */
    int num_of_section_headers;
    char buff[120];

    printf("enter a file name:\n");
    char *filename = fgets(buff, 120, stdin);
    sscanf(buff, "%s", filename); //remove enter char from the scan
    if (fd != -1)
    {
        close(fd);
    }
    if ((fd = open(filename, O_RDWR)) < 0)
    {
        perror("error in open");
        exit(-1);
    }

    if (fstat(fd, &fd_stat) != 0)
    {
        perror("stat failed");
        exit(-1);
    }

    if ((map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
        perror("mmap failed");
        exit(-4);
    }

    /* now, the file is mapped starting at map_start.
    * all we need to do is tell *header to point at the same address:
    */

    header = (Elf32_Ehdr *)map_start;
    /* now we can do whatever we want with header!!!!
    * for example, the number of section header can be obtained like this:
    */
    printf("%c", header->e_ident[1]);   //E
    printf("%c", header->e_ident[2]);   //L
    printf("%c\n", header->e_ident[3]); //F

    printf("Entry point address:\t\t%x\n", header->e_entry);
    if (header->e_ident[1] != 'E' || header->e_ident[2] != 'L' || header->e_ident[3] != 'F')
    {
        munmap(map_start, fd_stat.st_size);
        close(fd);
        fd = -1;
        printf("not ELF\n");
        return;
    }
    printf("version:\t\t\t%i\n", header->e_version);            //The data encoding scheme of the object file
    printf("Start of section headers:\t%i\n", header->e_shoff); //The file offset in which the section header table resides.
    num_of_section_headers = header->e_shnum;
    printf("Number of section headers:\t%i\n", num_of_section_headers); //The number of section header entries
    printf("Size of section headers:\t%i\n", header->e_shentsize);      //The size of each section header entry
    printf("Start of section headers:\t%i\n", header->e_phoff);         //The file offset in which the program header table resides.
    printf("Number of program headers:\t%i\n", header->e_phnum);        //The number of program header entries
    printf("Size of program headers: \t%i\n", header->e_phentsize);     //The size of each program header entry.

    /* now, we unmap the file */
    munmap(map_start, fd_stat.st_size);
}
void print_section_names()
{
    void *map_start; /* will point to the start of the memory mapped file */
    struct stat st;
    if (fstat(fd, &st) != 0)
    {
        perror("stat");
        return 1;
    }
    map_start = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;
    Elf32_Shdr *shdr = (Elf32_Shdr *)(map_start + ehdr->e_shoff);
    int shnum = ehdr->e_shnum;

    Elf32_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
    const char *const sh_strtab_p = map_start + sh_strtab->sh_offset;

    for (int i = 0; i < shnum; ++i)
    {
        printf("%2d: %s\t %i \t %i\t %i\t %i \n", i, sh_strtab_p + shdr[i].sh_name, shdr[i].sh_addr, shdr[i].sh_offset, shdr[i].sh_size, shdr[i].sh_type);
    }
    munmap(map_start, st.st_size);
}

void print_symbols()
{
    void *map_start; /* will point to the start of the memory mapped file */
    struct stat st;
    char *strtab;

    Elf32_Sym *symtab;
    Elf32_Sym *dyntab;
    if (fstat(fd, &st) != 0)
    {
        perror("stat");
        return 1;
    }

    map_start = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;                   //header
    Elf32_Shdr *shdr = (Elf32_Shdr *)(map_start + ehdr->e_shoff); //section
    Elf32_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
    Elf32_Shdr *sha_size;
    Elf32_Shdr *sha_size2;

    const char *const sh_strtab_p = map_start + sh_strtab->sh_offset;

    if (fd == -1)
    {
        printf("error ! there is no file open");
        return;
    }

    for (int i = 0; i < ehdr->e_shnum; i++)
    {

        if (strcmp(sh_strtab_p + shdr[i].sh_name, ".symtab") == 0)
        {
            symtab = (Elf32_Sym *)&(shdr[i]);
            sha_size = (Elf32_Shdr *)&(shdr[i]);
        }

        if (strcmp(sh_strtab_p + shdr[i].sh_name, ".dynsym") == 0)
        {
            dyntab = (Elf32_Sym *)&(shdr[i]);
            sha_size2 = (Elf32_Shdr *)&(shdr[i]);
        }

        if (strcmp(sh_strtab_p + shdr[i].sh_name, ".strtab") == 0)
            strtab = (char *)(map_start + shdr[i].sh_offset);
    }
    int count2 = sha_size2->sh_size / sha_size2->sh_entsize;
    int count = sha_size->sh_size / sha_size->sh_entsize;
    printf("symbol table :\n");
    for (size_t i = 0; i < count; i++)
        printf("[%i]\t%d\t%s\n", i, symtab[i].st_info, strtab + symtab[i].st_name);

    printf("\n dyntab table :\n");
    for (size_t i = 0; i < count2; i++)
        printf("[%i]\t%d\t%s\n", i, dyntab[i].st_info, strtab + dyntab[i].st_name);
}
// void relocation_tables ()
// {
//     struct stat st;
//     char* addr=mmap(0, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//     Elf32_Ehdr *ehdr = (Elf32_Ehdr*)addr;

//     Elf32_Shdr *shdr = (Elf32_Shdr *)(addr + ehdr->e_shoff);
//      int shnum = ehdr->e_shnum;

//     Elf32_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
//     const char *const sh_strtab_p = addr + sh_strtab->sh_offset;
//     int idodoindex=0;
//     Elf32_Shdr *cur_sh;
//     Elf32_Shdr *cur_shReal;//rel.dyn
//     Elf32_Shdr *strrrr_sh;
//     Elf32_Shdr *dysm;


//     for (size_t i = 0; i < shnum; i++)
//     {
//         cur_sh=&shdr[i];

//         if(cur_sh->sh_type ==SHT_STRTAB)
//          {
//             strrrr_sh = &shdr[i];
            
//          }
//          if(cur_sh->sh_type ==SHT_DYNSYM)
//          {
//             dysm = &shdr[i];
//          }


//     }

//     for (size_t i = 0; i < shnum; i++)
//     {
//         cur_sh=&shdr[i];
//          if(cur_sh->sh_type==SHT_REL)
//          {         
//              cur_shReal=cur_sh;
//              idodoindex=i;
         
//         Elf32_Shdr *sh_relTab=&shdr[idodoindex];
//         int numOfIdodods=cur_shReal->sh_size/sizeof(Elf32_Rel);
//         Elf32_Rel *relPtr=(Elf32_Rel*)(addr+sh_relTab->sh_offset);

//         for(int j=0; j<numOfIdodods; j++){
//             printf("[%2d] ", j);
//             printf("%X\t",relPtr->r_offset);
//             printf("%X\t",relPtr->r_info);
//             int info=relPtr->r_info;
//             printf("%x\t",ELF32_R_TYPE(info));
//             int symOff=ELF32_R_SYM(info);
//             // int dysmOff=addr+dysm->sh_offset;
//             Elf32_Sym *symPtr=(Elf32_Sym*)(addr + dysm->sh_offset);
//             symPtr=symPtr+symOff;
//             int value=symPtr->st_value;

//             //printf("%x\t",addr + strrrr_sh->sh_offset + value );

//             printf("the offset issss %x\t", strrrr_sh->sh_offset );


//             printf("%x\t", value );
            
//             // int name = symPtr->st_name;

//             printf("%s\t",addr + strrrr_sh->sh_offset + symPtr->st_name );


//             // printf("%x\t",ELF32_R_SYM(info));

//             printf("\n");

//             relPtr++;
//         }

//         printf("Number of entries rels:%d\n",numOfIdodods);
//          }
//     }
// }
void relocation_tables()
{
    void *map_start; /* will point to the start of the memory mapped file */
    struct stat st;
    char *strtab;
    char *strtab2;
    int symtab_Ind;
    Elf32_Rel *reltab;
    Elf32_Rel *reltab2;
    if (fstat(fd, &st) != 0)
    {
        perror("stat");
        return 1;
    }

    map_start = mmap(0, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);

    Elf32_Ehdr *ehdr = (Elf32_Ehdr *)map_start;                   //header
    Elf32_Shdr *shdr = (Elf32_Shdr *)(map_start + ehdr->e_shoff); //section
    Elf32_Shdr *sh_strtab = &shdr[ehdr->e_shstrndx];
    Elf32_Shdr *sha_size;
    Elf32_Shdr *sha_size2;
    Elf32_Sym *symtab;
    

    const char *const sh_strtab_p = map_start + sh_strtab->sh_offset;

    if (fd == -1)
    {
        printf("error ! there is no file open");
        return;
    }

    for (int i = 0; i < ehdr->e_shnum; i++)
    {

        if (strcmp(sh_strtab_p + shdr[i].sh_name, ".rel.plt") == 0)
        {
            reltab = (Elf32_Rel *)(map_start+shdr[i].sh_offset);
            sha_size = (Elf32_Shdr *)&(shdr[i]);
        }
        if (strcmp(sh_strtab_p + shdr[i].sh_name, ".rel.dyn") == 0)
        {
            reltab2 = (Elf32_Rel *)(map_start+shdr[i].sh_offset);
            sha_size2 = (Elf32_Shdr *)&(shdr[i]);
        }
        if (strcmp(sh_strtab_p + shdr[i].sh_name, ".dynsym") == 0)
            symtab = (Elf32_Sym *)(map_start+shdr[i].sh_offset);
        if (strcmp(sh_strtab_p + shdr[i].sh_name, ".dynstr") == 0)
            strtab2 = (char *)(map_start + shdr[i].sh_offset);
    }
    int count = sha_size->sh_size / sha_size->sh_entsize;
        printf("Relocation section .rel.dyn\n");
        printf("Offset Info Type Sym.Value Sym.Name\n");
        printf("\n%i\n",count);
    for (int i = 0; i < count; i++)
    {   
        printf("[%d]",i);
        printf("0x%-10x" ,reltab[i].r_offset);
        printf("0x%02x ", reltab[i].r_info);
        printf("%d ", ELF32_R_TYPE(reltab[i].r_info));
        symtab_Ind = ELF32_R_SYM(reltab[i].r_info);
        printf("%d ", symtab[symtab_Ind].st_value);
        printf("%s\n", symtab[symtab_Ind].st_name+strtab2);
    }
    count = sha_size2->sh_size / sha_size2->sh_entsize;
    printf("\n%i\n",count);
    printf("Relocation section .rel.sym\n");
    printf("Offset Info Type Sym.Value Sym.Name\n");
    for (int i = 0; i < count; i++)
    {   
        printf("[%d]",i);
        printf("0x%-10x" ,reltab2[i].r_offset);
        printf("0x%02x ", reltab2[i].r_info);
        printf("%d ", ELF32_R_TYPE(reltab2[i].r_info));
        symtab_Ind = ELF32_R_SYM(reltab2[i].r_info);
        printf("%d ", symtab[symtab_Ind].st_value);
        printf("%s\n", symtab[symtab_Ind].st_name+strtab2);
    }
}
void quit()
{
    close(fd);
    exit(0);
}

int main(int argc, char **argv)
{
    // argvAtOne=argv[1];
    // state *s = (state *)calloc(sizeof(state), 1);
    //open file to handel the bais case
    struct fun_desc menu[] = {{"Toggle Debug Mode", toggle_debug_mode},
                              {"Examine ELF File", examine_ELF_file},
                              {"Print Section Names", print_section_names},
                              {"Print Symbols", print_symbols},
                              {"Relocation Tables", relocation_tables},
                              {"Quit", quit},
                              {NULL, NULL}};
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
        menu[num - 1].fun();
        printf("Done\n");
    }
    return 0;
}
