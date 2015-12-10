#include "assembly.h"
#include "stack.h"

#ifdef _WIN32

char* introassembly = 
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
"	movl $0, %ecx\n";

char* exitassembly = 
"	push %edx\n"
"	call _free\n"
"   pop %edx\n";

#else

char* introassembly =
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
"	movl $0, %ecx\n";

char* exitassembly =
"	push %edx\n"
"	call free\n"
"   pop %edx\n";

#endif

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

    fputs(introassembly,fd);

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
"   popl %ebx\n"
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
    fputs(exitassembly,fd);
    fputs(
"   leave\n"
"	ret\n",fd);
    return 1;
}