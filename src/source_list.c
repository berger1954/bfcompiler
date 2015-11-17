#include <stdio.h>
#include <stdlib.h>
#include "source_list.h"

int insert(list* sourcelist, char token, int count)
{
    node* newnode = (node*)malloc(sizeof(node));
    newnode->next = NULL;
    newnode->token = token;
    newnode->count = count;

    if(sourcelist->head == NULL)
    {
        sourcelist->head = newnode;
        sourcelist->tail = newnode;   
    }
    else
    {
        sourcelist->tail->next = newnode;
        sourcelist->tail = newnode; 
    }
    return 1;    
}

int length(list* sourcelist)
{
    return sourcelist->length;
}

int next(list* sourcelist)
{
    if (sourcelist->currentnode->next != NULL)
    {
        sourcelist->currentnode = sourcelist->currentnode->next;
        return 1;
    }
    return 0;
}

node* currentnode(list* sourcelist)
{
    return sourcelist->currentnode;
}

int resetcurrent(list* sourcelist)
{
    sourcelist->currentnode = sourcelist->head;
    return 0;
}

list* createlist()
{
    list* newlist = (list*)malloc(sizeof(struct source_list));
    if (newlist != NULL)
    {
        newlist->head = NULL;
        newlist->currentnode = NULL;
        newlist->tail = NULL;
        newlist->length = 0;
    }
    return newlist;
}

void destroylist(list* sourcelist)
{
    node* currentnode = sourcelist->head;
    while (currentnode != NULL)
    {
        node* tempnode = currentnode->next;
        free(currentnode);
        currentnode = tempnode;
    }
    free(sourcelist);
    sourcelist = NULL;
}

void printlist(list* sourcelist)
{
    node* currentnode = sourcelist->head;
    while(currentnode != NULL)
    {
        printf("%c, %d\n", currentnode->token, currentnode->count);
        currentnode = currentnode->next;
    }
}
