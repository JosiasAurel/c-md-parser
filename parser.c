#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Node
{
    struct Node* prev;
    struct Node* next;
    char value[100];
};

typedef struct 
{
    int count; // represents the number of nodes in the linked list
    struct Node* head;
    struct Node* tail;
} LL;

LL ll = { .head = NULL, .tail = NULL, .count = 0 };

struct Node* create_node(const char* value)
{
    struct Node* node = malloc(sizeof(struct Node));
    strcpy(node->value, value);
    return node;
}

void append(const char* value)
{
    ll.count++;
    struct Node* new_node = create_node(value);
    new_node->next = NULL;
    if (ll.head == NULL)
    {
        new_node->prev = NULL;
        ll.head = new_node;
        ll.tail = new_node;
        return;
    }
    // ll.head = new_node;
    new_node->prev = ll.tail;
    ll.tail->next = new_node;
    ll.tail = new_node;
    return;
}

struct Node* node_at(int index)
{
    struct Node* curr_node;
    if (ll.head != NULL && index > 0 && index <= ll.count) { curr_node = ll.head; } else { return NULL; }
    int count = 0;
    while (count < index-1 && curr_node != NULL)
    {
        curr_node = curr_node->next; 
        count++;
    }
    return curr_node;
}

// insert the value at the specified index
// if there is a node at the position, it will push that node forward
// if there is no node at that index, it will return -1
int insert_before(int index, char* value) {
    struct Node* node = create_node(value);
    struct Node* node_at_index = node_at(index);
    if (node_at_index != NULL) 
    {
        struct Node* prev = node_at_index->prev; 
        node_at_index->prev = node;
        if (prev != NULL) prev->next = node;
        node->next = node_at_index;
        node->prev = prev != NULL ? prev : NULL;
        if (prev == NULL) ll.head = node;
    } else return -1;
    ll.count++;
    return 0;
}

int insert_after(int index, char* value)
{
    struct Node* node = create_node(value);
    struct Node* node_at_index = node_at(index);
    if (node_at_index != NULL)
    {
        struct Node* next = node_at_index->next;
        node_at_index->next = node;
        if (next != NULL) next->prev = node;
        node->prev = node_at_index;
        node->next = next != NULL ? next : NULL;
        if (next == NULL) ll.tail = node;
    } else return -1;
    ll.count++;
    return 0;
}

// thou shall silently crash
void delete(int index)
{
    struct Node* node = node_at(index);
    if (node != NULL)
    {
        if (node->prev == NULL)
        {
            if (node->next != NULL) 
            {
                node->next->prev = NULL;
                ll.head = node->next;
            }
        } 
        else
        {
            if (node->next != NULL) node->next->prev = node->prev;
            node->prev->next = node->next;
        }

        free(node); // PURGE!!!!!!!!!!!!! 
        ll.count--;
    }
}

void naive_traverse()
{
    struct Node* curr_node = ll.head;
    for (int i = 0; i < ll.count; i++)
    {
        printf("%s \n", curr_node->value);
        curr_node = curr_node->next;
    }
}

// reads text and transforms each piece of word into a Linked List
LL* text_as_ll(FILE* file)
{

}

int main(int argc, char* argv[]) {
    
    // check if there are 3 arguments
    // first is the binary file name
    // second and third should be the input and output file names 
    if (argc == 3) {} 
    else { printf("Not enough or too many arguments \n"); exit(1); }
     
    char* input_file = argv[1];
    char* output_file = argv[2];
   
    FILE* file = fopen(input_file, "r");

    return 0;
}

