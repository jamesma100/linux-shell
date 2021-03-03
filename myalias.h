#ifndef _myalias_h
#define _myalias_h
struct linked_list {
    struct node *head;
    struct node *tail;
};

struct node {
    void *key;
    void *value;
    struct node *next;
    struct node *prev;
};

typedef struct linked_list linked_list;
typedef struct node node;

void print_list(struct linked_list*);
struct node* insert_to_end(struct linked_list*, void*, void*);
struct node* delete(struct linked_list*, void*);
struct node* search(struct linked_list*, void*);

#endif
