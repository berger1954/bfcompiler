#include "stack.h"
#include <stdlib.h>

int push (struct stack* stack, int content)
{
    struct node* newnode;
    newnode = (struct node*)malloc(sizeof(struct node));
    newnode->content = content;
    newnode->next = stack->top;
    stack->top = newnode;
    stack->length += 1;
    return 1;
}

int pop(struct stack* stack)
{
    struct node* temptop = stack->top;
    int returnval = stack->top->content;
    struct node* nextnode = stack->top->next;
    stack->top = nextnode;
    stack->length -= 1;
    temptop->next = NULL;
    free(temptop);
    return returnval; 
}

int peak(struct stack* stack)
{
    return stack->top->content;
}

int isempty(struct stack* stack)
{
    if (stack->top == NULL)
    {
        return 1;
    }
    return 0;
}
