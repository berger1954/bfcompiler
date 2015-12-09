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

int writeAssembly(list* program, FILE* fd)
{
    struct stack* jumpstack = (struct stack*)malloc(sizeof(struct stack));
    jumpstack->top = NULL;
    jumpstack->length = 0;
    int jumplabel = 0;
    node* currentnode = program->head;
    if (currentnode == NULL || fd == NULL)
    {
        return 0;
    }
    #ifdef _WIN32
    fputs(
"    .file \"main.s\"\n"
"    .text\n"
".globl main\n"
"    .globl _main\n"
"    .global main\n"
"    .global _main\n"
"    .extern _malloc\n"
"	.extern _putchar\n"
"	.extern _getchar\n"
"_main:\n"
"main:\n"
"	pushl %ebp\n"
"	movl %esp, %ebp\n"
"   pushl $30000\n"
"	call _malloc\n"
"	movl %eax, %edx\n"
"clear:\n"
"	pushl $30000\n"
"	pushl $0\n"
"	pushl %edx\n"
"	call _memset\n"
"	popl %edx\n"
"	addl $8, %esp\n"
"   movl $0, %ebx\n"
"	movl $0, %ecx\n",fd);

    #else
    fputs(
".file \"main.s\"\n"
"	.text\n"
".globl main\n"
"	.globl _main\n"
"	.global main\n"
"	.global _main\n"
"	.extern malloc\n"
"	.extern putchar\n"
"	.extern getchar\n"
"_main:\n"
"main:\n"
"	pushl %ebp\n"
"	movl %esp, %ebp\n"
"   pushl $30000\n"
"	call _malloc\n"
"	movl %eax, %edx\n"
"clear:\n"
"	pushl $30000\n"
"	pushl $0\n"
"	pushl %edx\n"
"	call _memset\n"
"	popl %edx\n"
"	addl $8, %esp\n"
"   movl $0, %ebx\n"
"	movl $0, %ecx\n",fd);

    #endif
    while (currentnode != NULL)
    {
        if (currentnode->token == '+')
        {
            char numbuffer[10] = "";
            sprintf(numbuffer,"%d",currentnode->count);
            fputs("\taddb $",fd);
            fputs(numbuffer,fd);
            fputs(", (%edx, %ebx)\n",fd);
        } 
        else if (currentnode->token == '-')
        {
            char numbuffer[10] = "";
            sprintf(numbuffer,"%d",currentnode->count);
            fputs("\tsubb $",fd);
            fputs(numbuffer,fd);
            fputs(", (%edx, %ebx)\n",fd);
        }
        else if (currentnode->token == '<')
        {
            char numbuffer[10] = "";
            sprintf(numbuffer,"%d",currentnode->count);
            fputs("\tsubl $",fd);
            fputs(numbuffer,fd);
            fputs(", %ebx\n",fd);
        }       
        else if (currentnode->token == '>')
        {
            char numbuffer[10] = "";
            sprintf(numbuffer,"%d",currentnode->count);
            fputs("\taddl $",fd);
            fputs(numbuffer,fd);
            fputs(", %ebx\n",fd);
        } 
        else if (currentnode->token == '.')
        {
            #ifdef _WIN32
            fputs("\tmovzbl (%edx, %ebx), %eax\n"
"	pushl %edx\n"
"	pushl %ebx\n"
"	pushl %eax\n"
"   call _putchar\n"
"	popl %ecx\n"
"	popl %ebx\n"
"	popl %edx\n",fd);

            #else
            fputs("\tmovzbl (%edx, %ebx), %eax\n"
"	pushl %edx\n"
"	pushl %ebx\n"
"	pushl %eax\n"
"   call putchar\n"
"	popl %ecx\n"
"	popl %ebx\n"
"	popl %edx\n",fd);
            #endif
        }      
        else if (currentnode->token == ',')
        {
            #ifdef _WIN32
            fputs("\tpushl %edx\n"
"	pushl %ebx\n"
"	call _getchar\n"
"   popl %ebx\n"
"	popl %edx\n"
"	movb %al, (%edx, %ebx)\n",fd);
            #else
            fputs("\tpushl %edx\n"
"	pushl %ebx\n"
"	call getchar\n"
"    popl %ebx\n"
"	popl %edx\n"
"	movb %al, (%edx, %ebx)\n",fd);
            #endif
        }
        else if (currentnode->token == '[')
        {
            char labelbuffer1[15] = "";
            char labelbuffer2[15] = "";
            sprintf(labelbuffer1, "%c%d",'L', jumplabel);
            jumplabel++;
            sprintf(labelbuffer2, "%c%d",'L', jumplabel);
            push(jumpstack, jumplabel);
            jumplabel++;
            fputs("\tcmpb $0, (%edx, %ebx)\n",fd);
            fputs("\tje ",fd);
            fputs(labelbuffer2, fd);
            fputs("\n",fd);
            fputs(labelbuffer1, fd);
            fputs(":\n", fd);
        }
        else if (currentnode->token == ']')
        {
            char labelbuffer1[15] = "";
            char labelbuffer2[15] = "";
            int tempjumplabel1 = pop(jumpstack);
            int tempjumplabel2 = tempjumplabel1 - 1;
            sprintf(labelbuffer1, "%c%d",'L', tempjumplabel1);
            sprintf(labelbuffer2, "%c%d",'L', tempjumplabel2);
            fputs("\tcmpb $0, (%edx, %ebx)\n",fd);
            fputs("\tjne ",fd);
            fputs(labelbuffer2, fd);
            fputs("\n",fd);
            fputs(labelbuffer1, fd);
            fputs(":\n", fd);
        }
        currentnode = currentnode->next;
    }
    #ifdef _WIN32
    fputs("\tpush %edx\n"
"	call _free\n"
"   pop %edx\n",fd);
    #else
    fputs("\tpush %edx\n"
"	call free\n"
"   pop %edx\n",fd);
    #endif
    fputs("\tleave\n"
"	ret\n",fd);
    return 1;
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

    assert(fp > -1);

    assert(fstat(fp, &filestats) > -1);

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
    if (compile)
    {
        strcat(systemcommand, "as ");
        strcat(systemcommand, outputfile);
        strcat(systemcommand, " -o ");
        strcat(systemcommand, outname);
        system(systemcommand);
    }

    return 0;    
}
