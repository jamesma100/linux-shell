#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "myalias.h"
/*
struct linked_list {
    struct node *head;
    struct node *tail;
};
struct node {
    void *key;
    void *value;
    struct node *next;
    struct node *prev;
}; */
void print_list(struct linked_list *list) {
    struct node* curr = list->head;
    while (curr != NULL) {
        write(STDOUT_FILENO, (char*)(curr->key), strlen((char*)curr->key));
        write(STDOUT_FILENO, " ", 1);
        write(STDOUT_FILENO, (char*)(curr->value), strlen((char*)curr->value));
        write(STDOUT_FILENO, "\n", 1);
        //printf("%s %s\n", (char*)(curr->key), (char*)(curr->value));
        curr = curr->next;
    }
}
struct node* insert_to_end(struct linked_list *list, void *key, void *value) {
    struct node* new_node = (struct node*) malloc(sizeof(struct node));
    // initialize new node
    new_node->key = key;
    new_node->value = value;
    new_node->next = NULL;
    new_node->prev = NULL;
    // make new node the head and tail
    if (list->head == NULL) {
        list->head = new_node;
        list->head->next = NULL;
        list->tail = new_node;
    } else {
        struct node* curr = list->head;
        while (curr->next != NULL) {
            curr = curr->next;   
        }
        curr->next = new_node;
        new_node->prev = curr;
        new_node->next = NULL;
        list->tail = new_node;
    }
    return new_node;
}

struct node* delete(struct linked_list *list, void *key) {
    struct node* curr = list->head;
    
    // invalid list if head is NULL
    if (list->head == NULL) {
        return NULL;
    }
    // traverse list
    while (strcmp(curr->key, key) != 0) {
        // reach end of w/o finding match
        if (curr->next == NULL) {
            return NULL;
        } 
        else {
            curr = curr->next;
        }
    }
    // found node to delete
    if (curr == list->head) {
        list->head = list->head->next;
    } else {
        curr->prev->next = curr->next;
    }
    if (curr == list->tail) {
        list->tail = curr->prev;
    } else {
        curr->next->prev = curr->prev;
    } 
    return curr;    
}

struct node* search(struct linked_list *list, void *key) {
    struct node* curr = list->head;
    if (curr == NULL) {
        return NULL;
    }
    if (strcmp((char*)(curr->key), (char*)key) == 0) {
        return curr;
    }
    while (strcmp((char*)(curr->key), (char*)key) != 0) {
        if (curr->next == NULL) {
            return NULL;
        } else {
            curr = curr->next;
        }
    }
    if (curr == list->head) {
        return NULL;
    }
    return curr;
}
/*
int main(int argc, char *argv[]) {
    struct linked_list* list = malloc(sizeof(struct linked_list));
    list->head = NULL;
    list->tail = NULL;
    insert_to_end(list, "apple", "red");
    insert_to_end(list, "banana", "yellow");
    insert_to_end(list, "grape", "purple");
    insert_to_end(list, "orange", "orange");
    print_list(list);
    printf("value of search for grape: %s\n", (char*)((search(list, "grape"))->value));
    printf("value of search for apple: %s\n", (char*)((search(list, "apple"))->value));
    
    //print_list(list);
    //printf("list head key: %s\n", (char*)(list->head->key));
    return 0;
}
*/
