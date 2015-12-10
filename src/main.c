#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "source_list.h"
#include "stack.h"
#include "assembly.h"

int verify(char* program)
{
    int verifystack = 0;
    int i = 0;    
    while(program[i] != '\0')
    {
        if (program[i] == '[')
        {
             verifystack++;
        }
        else if (program[i] == ']')
        {
            verifystack--;
        }

    i++;
    }
    if (verifystack == 0)
    {
        return 0;
    }
    return -1;
}

list* optimize(char* program)
{
    int i = 0;
    int j = 0;
    list* optimized = createlist();
    while (program[i] != '\0')
    {
        if (program[i] == '+')
        {
            int temp = 0;
            while(program[i] == '+')
            {
                temp++;
                i++;
            }
            i--;
            insert(optimized, program[i], temp);
        }
        else if (program[i] == '-')
        {
            int temp = 0;
            while(program[i] == '-')
            {
                temp++;
                i++;
            }
            i--;
            insert(optimized, program[i], temp);
        }
        else if (program[i] == '>')
        {
            int temp = 0;
            while(program[i] == '>')
            {
                temp++;
                i++;
            }
            i--;
            insert(optimized, program[i], temp);
          }
        else if (program[i] == '<')
        {
            int temp = 0;
            while(program[i] == '<')
            {
                temp++;
                i++;
            }
            i--;
            insert(optimized, program[i], temp);
       }
       else 
       {
           insert(optimized, program[i], 1);         
       }
       i++;
    } 
    return optimized;
}

int main(int argc, char* argv[])
{
    int fp;
    char* program;
    struct stat filestats;
    char* sourcename = "";
    char outputfile[256] = "a.s";
    char* outname;
    int buffersize = 30000;
    int mode = 32;
    char systemcommand[256];
    int compile = 1;
    int assemble = 0;

    if (argc < 2)
    {
        printf("Usage <source file>.b [-i input file]|[-o output file]|[-s buffer size]|[-m32 32 bit mode]|[-m64 64 bit mod]\n");
        exit(1);
    }

    for (int i = 1; i < argc; i++)
    {
        if ((strcmp(argv[i], "-i") == 0) && (i < argc - 1))
        {
            sourcename = argv[i + 1];
            i++;
        }
        else if ((strcmp(argv[i], "-o")) == 0 && (i < argc - 1)) 
        {
            strcpy(outputfile, argv[i + 1]);
            strcat(outputfile, ".s");
            outname = argv[i + 1];
            i++;
        }
        else if ((strcmp(argv[i], "-s")) == 0 && (i < argc - 1))
        {
            buffersize = atoi(argv[i + 1]);
            i++;
            printf("Not yet implemented. Sorry.\n");
        }
        else if ((strcmp(argv[i], "-m64")) == 0 && (i < argc - 1))
        {
            printf("64 bit mode not implemented yet. Sorry.\n");
            i++;
        }
        else if ((strcmp(argv[i], "-m32")) == 0 && (i < argc - 1))
        {
            printf("Good job choosing 32 bit mode. Cause that's the only version so far\n");
            i++;
        }
    }

    if (strcmp(sourcename, "") == 0)
    {
        sourcename = argv[1];
    }

    fp = open(sourcename, O_RDONLY);

    if (fp <= -1)
    {
        printf("Source file not opened. Compilation failed\n");
        return -1;
    }

    if (fstat(fp, &filestats) <= -1)
    {
        printf("Fstat operation failed.\n");
        return -1;
    }

    program = (char*)malloc(filestats.st_size);

    assert(program != NULL);

    assert(read(fp, program, filestats.st_size) >= 0);

    if (verify(program) != 0)
    {
        printf("Program brackets not closed\n");
        return -1;
    }

    FILE* output = fopen(outputfile, "w"); 
    list* optimizedprogram = optimize(program);
    writeAssembly(optimizedprogram, output);
    fclose(output);
    if (assemble)
    {
        strcat(systemcommand, "as ");
        strcat(systemcommand, outputfile);
        strcat(systemcommand, " -o ");
        strcat(systemcommand, outname);
        system(systemcommand);
    }

    return 0;    
}
