#include "trie.h"
#include <stdlib.h>

#define TRIE_BASE_CHAR ' '
struct trie_node {
    void * end_of_str_ctx;
    struct trie_node * next[95];
};

struct trie {
    struct trie_node * next[95];
};


static struct trie_node *internal_trie_exists(struct trie_node * node_i,const char * string) {
    while(node_i) {
        if(*string == '\0' && node_i->end_of_str_ctx != NULL) {
            return node_i;
        }
        node_i = node_i->next[(*string) - TRIE_BASE_CHAR];
        string++;
    }
    return NULL;
}


Trie trie() {
    return calloc(1,sizeof(struct trie));
}

const char *trie_insert(Trie trie,const char *string,void * end_of_str_ctx) {
    struct trie_node ** iterator = &trie->next[(*string) - TRIE_BASE_CHAR];
    while(1) {
        if(*iterator == NULL) {
            (*iterator) = calloc(1,sizeof(struct trie_node));
            if(*iterator == NULL)
                return NULL;
        }
        string++;
        if(*string !='\0')
            iterator = &(*iterator)->next[(*string) - TRIE_BASE_CHAR];
        else
            break;
    }
    (*iterator)->end_of_str_ctx = end_of_str_ctx;
    return string;
}
void trie_delete(Trie trie,const char *string) {
    struct trie_node * find_node;
    if(string == NULL)
        return;
    find_node = internal_trie_exists(trie->next[(*string) - TRIE_BASE_CHAR],string+1);
    if(find_node)
        find_node->end_of_str_ctx = NULL;
}

void * trie_exists(Trie trie,const char *string) {
    struct trie_node * find_node;
    if(string == NULL)
        return NULL;
    find_node = internal_trie_exists(trie->next[(*string) - TRIE_BASE_CHAR],string+1);
    return find_node == NULL ? NULL : find_node->end_of_str_ctx;
}

static void trie_destroy_sub(struct trie_node * node_i) {
    int i;
    for(i=0;i<95;i++) {
        if(node_i->next[i] != NULL) {
            trie_destroy_sub(node_i->next[i]);
            node_i->next[i] = NULL;
        }
    }
    free(node_i);
}
void trie_destroy(Trie * trie) {
    int i;
    if(*trie != NULL) {
        Trie t = *trie;
        for(i=0;i<95;i++) {
            if(t->next[i] != NULL) 
                trie_destroy_sub(t->next[i]);
        }
        free(*trie);
        (*trie) = NULL;
    }
}