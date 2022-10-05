#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// emscripten magic
#include <emscripten.h>

struct Node
{
    struct Node* prev;
    struct Node* next;
    char value[20];
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


int append(const char* value)
{
    ll.count++;
    struct Node* new_node = create_node(value);
    new_node->next = NULL;
    if (ll.head == NULL)
    {
        new_node->prev = NULL;
        ll.head = new_node;
        ll.tail = new_node;
        return 0;
    }
    // ll.head = new_node;
    new_node->prev = ll.tail;
    ll.tail->next = new_node;
    ll.tail = new_node;
    return 0;
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

int delete(int index)
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
    return 0;
}

// this will free all memory allocated

void clear_ll()
{
    for (int i = 0; i < ll.count; i++)
    {
        delete(i+1);
    }
}


int naive_traverse()
{
    struct Node* curr_node = ll.head;
    for (int i = 0; i < ll.count; i++)
    {
        printf("%s", curr_node->value);
        curr_node = curr_node->next;
    }
    return 0;
}

char content_in[10000];
char content_out[10000];

// reads text and writes each word into a Linked List
void text_as_ll()
{
    char buffer[20];
    int idx = 0;
    int cid = 0;
    char c = content_in[cid];
    while (c != '\0')
    {
        // printf("Value of C = %c \n", c);
        if (
                c != ' ' 
                && c != '\n' 
                && c != '*' 
                && c != '_' 
                && c != '~' 
                && c != '`')
        {
            buffer[idx] = c;
            idx++;
        } 
        else
        {
            // printf("Buffer = %s \n", buffer);
            append(buffer);
            idx = 0;
            for (int i = 0; i < 20; i++)
            {
                buffer[i] = 0;
            }
            if (c == '\n') append("\n");
            if (c == ' ') append(" ");
            if (c == '*') append("*");
            if (c == '_') append("_");
            if (c == '~') append("~");
            if (c == '`') append("`");
        }
        cid++;
        c = content_in[cid];
    }
    // printf("%s \n", buffer);
    append(buffer);
    append("\n");
}

// converts headings to corresponding html tags

void match_headings(int idx)
{
    int h_count = 0;
    // count the number of headings
    struct Node* node = node_at(idx);
    for (int i = 0; i < 4; i++)
    {
        if (node->value[i] == '#')
        {
            h_count++;
        } else break;
    }

    struct Node* next_node = node->next;
    int last_word_idx = idx+1; // index of last word before the next \n
    while (strcmp(next_node->value, "\n") != 0)
    {
        next_node = next_node->next;
        last_word_idx++;
    }

    char opening[6];
    char closing[6];
    
    sprintf(opening, "<h%d>", h_count);
    sprintf(closing, "</h%d>", h_count);
    
    insert_before(idx+1, opening);
    insert_after(last_word_idx, closing);
   
    delete(idx); // delete the hashtags
    delete(idx+1); // delete the whitespace after it
}

// handle blockquotes

void match_block(int idx)
{
    struct Node* node = node_at(idx);
    struct Node* next = node->next;
    int last_word_idx = idx+1;
    while (strcmp(next->value, "\n") != 0)
    {
        next = next->next;
        last_word_idx++;
    }

    strcpy(node->value, "<blockquote>");
    insert_after(last_word_idx, "</blockquote>");
}
// converts list into unordered list

void match_list(int idx)
{
    struct Node* node = node_at(idx);
    struct Node* next = node->next;
    int last_node_idx = idx+1;
    while (strcmp(next->value, "\n") != 0)
    {
        next = next->next;
        last_node_idx++;
    }
    strcpy(node->value, "<li>");
    insert_after(last_node_idx, "</li>");
    insert_after(last_node_idx+1, "\n");
}


// matches _,*,~,`,´´´

void match_duals(int idx)
{
    // find next special character
    struct Node* node = node_at(idx);
    struct Node* next = node->next;
    while (strcmp(next->value, "\n") != 0)
    { 
        if (strcmp(next->value, "*") == 0)
        {
            strcpy(node->value, "<b>");
            strcpy(next->value, "</b>");
            break;
        } else if (strcmp(next->value, "_") == 0)
        {
            strcpy(node->value, "<i>");
            strcpy(next->value, "</i>");
            break;
        }
        else if (strcmp(next->value, "~") == 0)
        {
            strcpy(node->value, "<strike>");
            strcpy(next->value, "</strike>");
            break;
        }
        else if (strcmp(next->value, "`") == 0)
        {
            strcpy(node->value, "<em>");
            strcpy(next->value, "</em>");
            break;   
        }

        next = next->next;
    } return;
}


void parse()
{
    struct Node* node = ll.head;
    int idx = 1;
    while (node->next != NULL)
    {
        if (
                strcmp(node->value, "*") == 0 
                || strcmp(node->value, "_") == 0 
                || strcmp(node->value, "~") == 0
                || strcmp(node->value, "`") == 0 
            )
        {
            match_duals(idx);
        }
        if (strcmp(node->value, "-") == 0)
        {
            if (idx == 1 || strcmp(node_at(idx-1)->value, "\n") == 0) match_list(idx);
        }
        if ((idx == 1 || strcmp(node_at(idx-1)->value, "\n") == 0)
                && strcmp(node->value, ">") == 0)
        {
            match_block(idx);
        }
        if ( (idx == 1 || strcmp(node_at(idx-1)->value, "\n") == 0)
                && node->value[0] == '#' 
                && (strcmp(node->next->value, " ") == 0)
            )
        {
            match_headings(idx);
        } else idx++; 
        node = node_at(idx);
        // idx++;
    }
}

void  join_and_out()
{
    int out_at = 0; // keeps track of which position at which we are in the content_out string
    struct Node* node = ll.head;
    for (int i = 0; i < ll.count && node != NULL; i++)
    {
        int length = strlen(node->value);
        for (int j = 0; j < length; j++)
        {
            content_out[out_at] = node->value[j];
            out_at++;
        }
        node = node->next;
    }
}

EMSCRIPTEN_KEEPALIVE
char* eat_and_poop(char* text_in)
{
    strcpy(content_in, text_in); // eat text
    text_as_ll();
    parse(); // digest
    join_and_out(); // poop
    // clear_ll();
    clear_ll(); ll.count = 0; ll.head = NULL; ll.tail = NULL; // just making sure
    return content_out;
}

