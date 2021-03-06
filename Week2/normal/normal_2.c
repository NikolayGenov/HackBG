#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_SIZE 256

typedef struct node {
    char* val;
    struct node* next;
} node_t;

void push(node_t* head, char* val) {
    node_t* current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    /* now we can add a new variable */
    current->next = malloc(sizeof(node_t));
    current->next->val = malloc(CHUNK_SIZE);
    strncpy(current->next->val, val, CHUNK_SIZE - 1);
    current->next->val[CHUNK_SIZE - 1] = '\0';
    current->next->next = NULL;
}

void delete_all_list(node_t* head) {
    node_t* current = head;

    while (current != NULL) {
        free(current->val);
        node_t* cur = current;
        current = current->next;
        free(cur);
    }
}
void print_list(node_t* head) {
    node_t* current = head;

    while (current != NULL) {
        printf("%s\n", current->val);
        current = current->next;
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fputs("Error - wrong number of argumets", stderr);
        exit(1);
    }
    char* name = argv[1];
    char buffer[CHUNK_SIZE];
    FILE* fp = fopen(name, "r");
    int i, flag = 1;

    if (fp == NULL) {
        fputs("Can't open input file \n", stderr);
        exit(1);
    }
    fseek(fp, 0, SEEK_SET);
    size_t bytesRead, bufSize = 1;

    node_t* head = NULL;
    head = malloc(sizeof(node_t));
    head->val = (char*)malloc(sizeof(char*));
    head->val[0] = '\0';
    head->next = NULL;

    do {
        bytesRead = fread(buffer, sizeof(char), CHUNK_SIZE - 1, fp);
        if (buffer == NULL) {
            fputs("Reading allocating memory\n", stderr);
            exit(1);
        }
        if (bytesRead != CHUNK_SIZE - 1) {
            flag = 0;
        }
        push(head, buffer);
        //  printf("%d\n",(int) pos);
        /* Do stuff here */
    } while (flag);

    print_list(head);

    // DELETE STUFF
    fclose(fp);
    delete_all_list(head);
    puts("Done ;)\n");
    return 0;
}
