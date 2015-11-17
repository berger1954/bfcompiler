#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "stack.h"
#include "source_list.h"

int verify(char* program)
{
    struct stack verifystack;
    verifystack.top = NULL;
    verifystack.length = 0;
    int i = 0;    
    while(program[i] != '\0')
    {
        if (program[i] == '[')
        {
             push(&verifystack, 1);
        }
        else if (program[i] == ']')
        {
             pop(&verifystack);
        }

    i++;
    }

    return isempty(&verifystack);
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
    fputs("\t.file \"main.s\"\n\t.text\n.globl main\n\t.globl _main\n\t.global main\n\t.global _main\n\t.extern _malloc\n\t.extern _putchar\n\t.extern _getchar\n",fd);
    fputs("_main:\nmain:\n\tpushl %ebp\n\tmovl %esp, %ebp\n", fd);  
    fputs("\tpushl $30000\n\tcall _malloc\n\tmovl %eax, %edx\n", fd);
    fputs("clear:\n\tpushl $30000\n\tpushl $0\n\tpushl %edx\n\tcall _memset\n\tpopl %edx\n\taddl $8, %esp\n", fd); 
    fputs("\tmovl $0, %ebx\n\tmovl $0, %ecx\n",fd);
    #else
    fputs(".code32\n",fd);
    fputs("\t.file \"main.s\"\n\t.text\n.globl main\n\t.globl _main\n\t.global main\n\t.global _main\n\t.extern malloc\n\t.extern putchar\n\t.extern getchar\n",fd);
    fputs("_main:\nmain:\n\tpushl %ebp\n\tmovl %esp, %ebp\n", fd);
    fputs("\tpushl $30000\n\tcall _malloc\n\tmovl %eax, %edx\n", fd);
    fputs("clear:\n\tpushl $30000\n\tpushl $0\n\tpushl %edx\n\tcall _memset\n\tpopl %edx\n\taddl $8, %esp\n", fd);
    fputs("\tmovl $0, %ebx\n\tmovl $0, %ecx\n",fd);
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
            fputs("\tmovzbl (%edx, %ebx), %eax\n\tpushl %edx\n\tpushl %ebx\n\tpushl %eax\n",fd);
            fputs("\tcall _putchar\n\tpopl %ecx\n\tpopl %ebx\n\tpopl %edx\n",fd);
            #else
            fputs("\tmovzbl (%edx, %ebx), %eax\n\tpushl %edx\n\tpushl %ebx\n\tpushl %eax\n",fd);
            fputs("\tcall putchar\n\tpopl %ecx\n\tpopl %ebx\n\tpopl %edx\n",fd);
            #endif
        }      
        else if (currentnode->token == ',')
        {
            #ifdef _WIN32
            fputs("\tpushl %edx\n\tpushl %ebx\n\tcall _getchar\n",fd);
            fputs("\tpopl %ebx\n\tpopl %edx\n\tmovb %al, (%edx, %ebx)\n",fd);
            #else
            fputs("\tpushl %edx\n\tpushl %ebx\n\tcall getchar\n",fd);
            fputs("\tpopl %ebx\n\tpopl %edx\n\tmovb %al, (%edx, %ebx)\n",fd);
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
    fputs("\tpush %edx\n\tcall _free\n",fd);
    #else
    fputs("\tpush %edx\n\tcall free\n",fd);
    #endif
    fputs("\tleave\n\tret\n",fd);
    return 1;
}


int main(int argc, char* argv[])
{
    int fp;
    char* program;
    struct stat filestats;

    if (argc < 2)
    {
        printf("Please specify source file\n");
        exit(1);
    }

    for (int i = 0; i < argc; i++)
    {
        
    }

    fp = open(argv[1], O_RDONLY);

    assert(fp > -1);

    assert(fstat(fp, &filestats) > -1);

    program = (char*)malloc(filestats.st_size);

    assert(program != NULL);

    assert(read(fp, program, filestats.st_size) >= 0);

    assert(verify(program) == 1);

    FILE* output = fopen("main.s", "w"); 
    list* optimizedprogram = optimize(program);
    writeAssembly(optimizedprogram, output);

    return 0;    
}
