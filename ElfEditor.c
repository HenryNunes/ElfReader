#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

typedef struct 
{
    //Identificador
    unsigned char  e_magic[4];
    unsigned char  e_class;
    unsigned char  e_data;
    unsigned char  e_versio;
    unsigned char  e_osabi;
    unsigned char  e_abiversion;
    unsigned char  e_pad[7];

    unsigned char e_type[2];
    unsigned char e_machine[2];
    unsigned char e_version[4];

    unsigned int e_entry_size;
    unsigned char* e_entry;
    unsigned int e_phoff_size;
    unsigned char* e_phoff ;
    unsigned int e_shoff_size;
    unsigned char* e_shoff;

    unsigned char e_flags[4];
    unsigned char e_ehsize[2];
    unsigned char e_phentsize[2];
    unsigned char e_phnum[2];
    unsigned char e_shentsize[2];
    unsigned char e_shnum[2];
    unsigned char e_shstrndx[2];
      
} ElfHeader;

typedef enum{
    e_ident,
    e_type,
    e_version,
    e_entry,
    e_phoff,
    e_shoff,
    e_flags,
    e_ehsize,
    e_phentsize,
    e_phnum,
    e_shentsize,
    e_shnum,
    e_shstrndx
} Atributo; 

ElfHeader* load(char* arq);
int save(ElfHeader* EH, char* nome);
int set(ElfHeader* EH, Atributo atr, uint64_t novoValor);
void printElfHeader(ElfHeader* EH);
unsigned char* getNumber(unsigned char* ptr,int size);

int ENDIANESS;
int header = 0;
char *target = NULL;
char *output = NULL;
char *atribute = NULL;
char *value = NULL;
int c;

int main(int argc, char **argv)
{

    while ((c = getopt (argc, argv, "ht:o:a:v:")) != -1)
    {
        switch (c)
        {
            case 'h':
                header = 1;
                break;
            case 't':
                target = optarg;
                break;
            case 'o':
                output = optarg;
                break;
            case 'a':
                atribute = optarg;
                break;
            case 'v':
                value = optarg;
                break;
        }

    }
    if(target == NULL)
    {
        printf("A target file is required\n");
        exit(-1);
    }

    ElfHeader* EH;
    EH = load(target);
    if(header == 1)
    {
        printElfHeader(EH);
    }

    // if(atribute != NULL && value != NULL)
    // {    
    //     set(EH, e_ident, );
    // }


    exit(0);
}

unsigned char* getNumber(unsigned char* ptr,int size)
{
    unsigned char* ret = malloc(size);
    if(ENDIANESS == 1)
    {
        for(int i = 0; i < size; i++)
        {
            *(ret + i) = *(ptr+i); 
        }
    }
    else
    {
        for(int i = 0; i < size; i++)
        {
            *(ret + i) = *(ptr + (size - i - 1));
        } 
    }
    return ret;
}

void printElfHeader(ElfHeader* EH)
{
    printf("Elf Header \n");
    printf("e_class: %x \n", EH->e_class);
    printf("e_data: %x \n", EH->e_data);
    printf("e_versio: %x \n", EH->e_versio);
    printf("e_osabi: %x \n", EH->e_osabi);
    printf("e_abiversion: %x \n", EH->e_abiversion);
    printf("e_type: %x \n", (int) *(getNumber(EH->e_type, 2)));
    printf("e_machine: %x \n", (int) *(getNumber(EH->e_machine, 2)));
    printf("e_version: %x \n", (int) *(getNumber(EH->e_version, 4)));
    if(EH->e_class == 1)
    {
        printf("e_entry: 0x%x \n", *((unsigned int*)(getNumber(EH->e_entry, EH->e_entry_size))));
        printf("e_phoff: %d \n", *((unsigned int*) (getNumber(EH->e_phoff, EH->e_phoff_size))));
        printf("e_shoff: %d \n", *((unsigned int*) (getNumber(EH->e_shoff, EH->e_shoff_size))));
    }
    else
    {
        printf("e_entry: 0x%lx \n", *((unsigned long*)(getNumber(EH->e_entry, EH->e_entry_size))));
        printf("e_phoff: %ld \n", *((unsigned long*) (getNumber(EH->e_phoff, EH->e_phoff_size))));
        printf("e_shoff: %ld \n", *((unsigned long*) (getNumber(EH->e_shoff, EH->e_shoff_size))));
    }
    printf("e_flags: 0x%x \n", (int) *(getNumber(EH->e_flags, 4)));
    printf("e_ehsize: %d \n", (int) *(getNumber(EH->e_ehsize, 2)));
    printf("e_phentsize: %d \n", (int) *(getNumber(EH->e_phentsize, 2)));
    printf("e_phnum: %d \n", (int) *(getNumber(EH->e_phnum, 2)));
    printf("e_shentsize: %d \n", (int) *(getNumber(EH->e_shentsize, 2)));
    printf("e_shnum: %d \n", (int) *(getNumber(EH->e_shnum, 2)));
    printf("e_shstrndx: %d \n", (int) *(getNumber(EH->e_shstrndx, 2)));
}

ElfHeader* load(char* arq)
{
    ElfHeader* ret;
    unsigned char* binary;

    //Tentando Abrir
    FILE *file;
    if((file = fopen(arq,"r")) == NULL)
    {
        perror("Error");
        exit(-1);
    }
    
    //Vendo tamanho
    fseek(file, 0, SEEK_END);
    unsigned long size = ftell(file);
    if(size < 54)
    {
        printf("File too Short");
        exit(-1);
    }
    fseek(file, 0, SEEK_SET);

    //Alocando objetor
    if((ret = malloc(sizeof(ElfHeader))) == NULL)
    {
        perror("Error");
        exit(-1);
    }

    if((binary = malloc(size)) == NULL)
    {
        perror("Error");
        exit(-1);
    }
    
    for (unsigned long i = 0; i < size; i++)
    {
        *(binary + i) = fgetc(file); 
    }
    fclose(file);

    unsigned char header[16] = {0x7f, 0x45, 0x4c, 0x46};
    for(int i = 0; i < 4; i++)
    {
        if(!(*(binary + i) == header[i]))
        {
            printf("Not and ELF file\n");
            exit(-1);
        } 
        ret->e_magic[i] = *(binary + i);
    }
    ret->e_class = *(binary + 4);
    ret->e_data = *(binary + 5);
    ENDIANESS = ret->e_data;
    ret->e_versio = *(binary + 6);
    ret->e_osabi = *(binary + 7);
    ret->e_abiversion = *(binary + 8);
    
    for(int i = 0; i < 7; i++)
    {
        ret->e_pad[i] = *(binary + 0x09 + i);
    }
    for(int i = 0; i < 2; i++)
    {
        ret->e_type[i] = *(binary + 0x10 + i);
    }
    for(int i = 0; i < 2; i++)
    {
        ret->e_machine[i] = *(binary + 0x12 + i);
    }
    for(int i = 0; i < 4; i++)
    {
        ret->e_version[i] = *(binary + 0x14 + i);
    }

    if(ret->e_class == 1)
    {
        ret->e_entry_size = 4;
        ret->e_entry = malloc(4);
        for(int i = 0; i < ret->e_entry_size; i++)
        {
            *(ret->e_entry + i) = *(binary + i + 0x18);   
        }

        ret->e_phoff_size = 4; 
        ret->e_phoff = malloc(4);
        for(int i = 0; i < ret->e_phoff_size; i++)
        {
            *(ret->e_phoff + i) = *(binary + i + 0x1C);   
        }

        ret->e_shoff_size = 4;
        ret->e_shoff = malloc(4);
        for(int i = 0; i < ret->e_shoff_size; i++)
        {
            *(ret->e_shoff + i) = *(binary + i + 0x20);   
        }

        for(int i = 0; i < 4; i++)
        {
            ret->e_flags[i] = *(binary + 0x24 + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_ehsize[i] = *(binary + 0x28 + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_phentsize[i] = *(binary + 0x2A + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_phnum[i] = *(binary + 0x2C + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_shentsize[i] = *(binary + 0x2E + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_shnum[i] = *(binary + 0x30 + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_shstrndx[i] = *(binary + 0x32 + i);
        }
    }
    else
    {
        ret->e_entry_size = 8;
        ret->e_entry = malloc(8);
        for(int i = 0; i < ret->e_entry_size; i++)
        {
            *(ret->e_entry + i) = *(binary + i + 0x18);   
        }

        ret->e_phoff_size = 8; 
        ret->e_phoff = malloc(8);
        for(int i = 0; i < ret->e_phoff_size; i++)
        {
            *(ret->e_phoff + i) = *(binary + i + 0x20);   
        }

        ret->e_shoff_size = 8;
        ret->e_shoff = malloc(8);
        for(int i = 0; i < ret->e_shoff_size; i++)
        {
            *(ret->e_shoff + i) = *(binary + i + 0x28);   
        }

        for(int i = 0; i < 4; i++)
        {
            ret->e_flags[i] = *(binary + 0x30 + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_ehsize[i] = *(binary + 0x34 + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_phentsize[i] = *(binary + 0x36 + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_phnum[i] = *(binary + 0x38 + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_shentsize[i] = *(binary + 0x3A + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_shnum[i] = *(binary + 0x3C + i);
        }
        for(int i = 0; i < 2; i++)
        {
            ret->e_shstrndx[i] = *(binary + 0x3E + i);
        }

    }

    free(binary);
    return ret;
}

int set(ElfHeader* EH, Atributo atr, uint64_t novoValor)
{

}