typedef struct source_nodes {
    char token;
    int count;
    struct source_nodes* next;
} node;

typedef struct source_list {
    node* head;
    node* currentnode;
    node* tail;
    int length;
} list;

int insert(list* sourcelist, char token, int count);

int length(list* sourcelist);

int next(list* sourcelist);

node* currentnode(list* sourcelist);

int resetcurrent(list* sourcelist);

void printlist(list* sourcelist);

list* createlist();

void destroylist(list* sourcelist);