struct node {
    struct node* next;
    int content;
};

struct stack {
    struct node* top;
    int length;
};

int push(struct stack* stack, int content);

int pop(struct stack* stack);

int isempty(struct stack* stack);

int peak(struct stack* stack);

