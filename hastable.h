#include "synoptic.h"

#ifndef HASTABLE_H
#define HASTABLE_H

typedef struct entry_s{
    element_t element;
    struct entry_s *next;
}entry_t;

typedef struct hashtable_s{
    int size;
    struct entry_s **table;
}hashtable_t;

hashtable_t *ht_create( int size );
int ht_hash( hashtable_t *hashtable, char *key );
entry_t *ht_newpair( char *key, char *value );
int ht_set( hashtable_t *hashtable, entry_t *newEntry );
entry_t *ht_get( hashtable_t *hashtable, char *key );
void ht_free(hashtable_t *hashtable);
#endif //HASTABLE_H
