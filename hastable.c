#include <limits.h>
#include "synoptic.h"
#include "hastable.h"


/* Create a new hashtable. */
hashtable_t * ht_create( int size ) {
    hashtable_t *hashtable = NULL;
    int i;

    if( size < 1 ) return NULL;

    /* Allocate the table itself. */
    if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) {
        return NULL;
    }

    /* Allocate pointers to the head nodes. */
    if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) {
        return NULL;
    }
    for( i = 0; i < size; i++ ) {
        hashtable->table[i] = NULL;
    }

    hashtable->size = size;

    return hashtable;
}

/* Hash a string for a particular hash table. */
int ht_hash( hashtable_t *hashtable, char *key )
{

    unsigned long int hashval = 0;
    int i = 0;

    /* Convert our string to an integer */
    while( hashval < ULONG_MAX && i < strlen( key ) ) {
        hashval = hashval << 8;
        hashval += key[ i ];
        i++;
    }

    return (int)(hashval % hashtable->size);
}

/* Insert a key-value pair into a hash table. */
int ht_set( hashtable_t *hashtable, entry_t *newEntry ) {
    int bin = 0;
    entry_t *next = NULL;
    entry_t *last = NULL;

    bin = ht_hash( hashtable, newEntry->element.key );

    next = hashtable->table[bin];

    while( next != NULL && strcmp( newEntry->element.key, next->element.key ) > 0 )
    {
        last = next;
        next = next->next;
    }

    /* There's already a pair.  Let's replace that string. */
    if( next != NULL && strcmp( newEntry->element.key, next->element.key ) == 0 )
    {
        // key already exist; don't over-write; return Error
        return ERROR_DUPLICATE_ENTRY;
    }
    else
    {
        /* We're at the start of the linked list in this bin. */
        if( next == hashtable->table[ bin ] ) {
            newEntry->next = next;
            hashtable->table[ bin ] = newEntry;

            /* We're at the end of the linked list in this bin. */
        } else if ( next == NULL ) {
            last->next = newEntry;

            /* We're in the middle of the list. */
        } else  {
            newEntry->next = next;
            last->next = newEntry;
        }
        return SUCCESS;
    }
}

/* Retrieve a key-value pair from a hash table. */
entry_t * ht_get( hashtable_t *hashtable, char *key ) {
    int bin = 0;
    entry_t *pair;

    bin = ht_hash( hashtable, key );

    /* Step through the bin, looking for our value. */
    pair = hashtable->table[ bin ];
    while( pair != NULL &&  strcmp( key, pair->element.key ) > 0 ) {
        pair = pair->next;
    }

    /* Did we actually find anything? */
    if( pair == NULL || strcmp( key, pair->element.key ) != 0 )
    {
        return NULL;
    } else {
        return pair;
    }
}
