#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mylib.h"
#include "htable.h"

struct htablerec{
    int num_keys;
    int capacity;
    int *freqs;
    int *stats;
    char **keys;
    hashing_t method;
};

/**
 * Prints out a line of data from the hash table to reflect the state
 * the table was in when it was a certain percentage full.
 * Note: If the hashtable is less full than percent_full then no data
 * will be printed.
 *
 * @param h - the hash table.
 * @param stream - a stream to print the data to.
 * @param percent_full - the point at which to show the data from.
 */
static void print_stats_line(htable h, FILE *stream, int percent_full) {
    int current_entries = h->capacity * percent_full / 100;
    double average_collisions = 0.0;
    int at_home = 0;
    int max_collisions = 0;
    int i = 0;

    if (current_entries > 0 && current_entries <= h->num_keys) {
        for (i = 0; i < current_entries; i++) {
            if (h->stats[i] == 0) {
                at_home++;
            } 
            if (h->stats[i] > max_collisions) {
                max_collisions = h->stats[i];
            }
            average_collisions += h->stats[i];
        }
    
        fprintf(stream, "%4d %10d %10.1f %10.2f %11d\n", percent_full, 
                current_entries, at_home * 100.0 / current_entries,
                average_collisions / current_entries, max_collisions);
    }
}

/**
 * Prints out a table showing what the following attributes were like
 * at regular intervals (as determined by num_stats) while the
 * hashtable was being built.
 *
 * @li Percent At Home - how many keys were placed without a collision
 * occurring.
 * @li Average Collisions - how many collisions have occurred on
 *  average while placing all of the keys so far.
 * @li Maximum Collisions - the most collisions that have occurred
 * while placing a key.
 *
 * @param h the hashtable to print statistics summary from.
 * @param stream the stream to send output to.
 * @param num_stats the maximum number of statistical snapshots to print.
 */
void htable_print_stats(htable h, FILE *stream, int num_stats) {
    int i;

    fprintf(stream, "\n%s\n\n", 
            h->method == LINEAR_P ? "Linear Probing" : "Double Hashing"); 
    fprintf(stream, "Percent   Current   Percent    Average      Maximum\n");
    fprintf(stream, " Full     Entries   At Home   Collisions   Collisions\n");
    fprintf(stream, "-----------------------------------------------------\n");
    for (i = 1; i <= num_stats; i++) {
        print_stats_line(h, stream, 100 * i / num_stats);
    }
    fprintf(stream, "-----------------------------------------------------\n\n");
}


/**
 * Calculates and returns an approprite index for hash table
 * insertion for a string.
 * @param word the word to calculate an insertion index for.
 * @return index the index of hash table insertion.
 */
static unsigned int htable_word_to_int(char *word){
    unsigned int index = 0;
    
    while(*word != '\0'){
        index = (*word++ + 31 * index);
    }
    
    return index;
}

/**
 * Calculates and returns the step used in double hashing
 * for given hash table and key index.
 * @param h the hash table.
 * @param i_key the index of the key for which a step will be calculated.
 * @return An integer value for the step used in double hashing.
 */
static unsigned int htable_step(htable h, unsigned int i_key){
    return 1 + (i_key % (h->capacity -1));
}

/**
 * Creates a new hash table with given size and hashing method.
 * @param c the capacity of the hash table.
 * @param t the type of hashing used, LINEAR_P for linear hashing
 * or DOUBLE_H for double hashing.
 * @return the newly created hash table.
 */
htable htable_new(int c, hashing_t t){
    int i;
    htable h = emalloc(sizeof *h);

    h->capacity = c;
    h->num_keys = 0;
    h->method = t;

    h->freqs = emalloc(c * sizeof h->freqs);
    h->keys = emalloc(c * sizeof h->keys);
    h->stats = emalloc(c * sizeof h->stats);

    for(i = 0; i < c; i++){
        h->freqs[i] = 0;
        h->keys[i] = NULL;
	h->stats[i] = 0;	
    }
    
    return h;
}

/**
 * Frees all memory associated with given hash table.
 * @param h The hash table to be freed.
 */
void htable_free(htable h){
    int i;
    
    for(i = 0; i < h->capacity; i++){
        if(h->keys[i] != NULL) free(h->keys[i]);
    }
    
    free(h->stats);
    free(h->keys);
    free(h->freqs);
    free(h);
}

/**
 * Inserts a key into given hash table.
 * @param h the hash table that the key will be inserted into.
 * @param s the key to be inserted.
 * @return 1- if the key was successfully inserted,
 *         0- if the hash table if full and the key cannot be inserted,
 *         the frequency of key insertion - if the key has already been
 *         inserted.
 */
int htable_insert(htable h, char *s){
    unsigned int index = htable_word_to_int(s) % h->capacity;
    int collisions = 0;

    /* Until capacity number of collisions, keep trying to insert. */
    for(;;){
        /* If the space is unoccupied, insert key here. */
        if(h->keys[index] == NULL){
            h->keys[index] = emalloc((strlen(s)+1) * sizeof h->keys[0][0]);
            strcpy(h->keys[index], s);
            h->freqs[index]++;
	    h->stats[h->num_keys] = collisions;
	    h->num_keys++;
            return 1;
            /* If the same key is found, increment frequency at that index. */
        } else if(strcmp(h->keys[index],s) == 0){
            h->freqs[index]++;
            return h->freqs[index];
        }
        /* Choose probing method */
        if(h->method == LINEAR_P) index++;
	if(h->method == DOUBLE_H) index += htable_step(h, index);
        index %= h->capacity;
	collisions++;
        /* Return 0 if capacity number of collisions. */
        if(collisions == h->capacity) return 0;
    }   
}

/**
 * Applies given function to every non-NULL item in the hash table.
 * @param h The hash table
 * @param f The function that will be applied
 */
void htable_print(htable h, void f(int freq, char *word)){
    int i;
    
    for(i = 0; i < h->capacity; i++){
        if (h->freqs[i] != 0) f(h->freqs[i], h->keys[i]);
    }
}

/**
 * Prints the entire contents of the hash table including
 * frequencies, stats, and keys.
 * @param h The hash table.
 */
void htable_print_entire_htable(htable h){
    int i;
    fprintf(stderr, "  Pos  Freq  Stats  Word\n ---------\
-------------------------------\n");
    for(i = 0; i < h->capacity; i++){
        if(h->keys[i] != NULL){
            fprintf(stderr,"%5d %5d %5d   %s\n",
                    i, h->freqs[i], h->stats[i], h->keys[i]);
        } else {
            fprintf(stderr,"%5d %5d %5d\n", i, h->freqs[i], h->stats[i]);
	}
    }
}

/**
 * Searches the hash table for given key.
 * @param h The hash table to be searched.
 * @param word The key to search for.
 * @return 0- If the key is never found
 *         If the key is found then the frequency of the key is returned.
 */
int htable_search(htable h, char *word){
    int collisions = 0;
    int index = htable_word_to_int(word) % h->capacity;

    for(;;){
        /* If that key doesn't exist in the table, break loop */
        if(h->keys[index] == NULL){
            index ++;
            break;
        }
        /* If the key is found, return the index of it. */
        if(strcmp(h->keys[index], word) == 0) return h->freqs[index];
        
        /* Depending on hashing method, increment index accordingly */
        if(h->method == LINEAR_P) index++;
	if(h->method == DOUBLE_H) index += htable_step(h, index);
        
	collisions++;
        index %= h->capacity;

        /* Break the loop if key not found after searching  whole table */
        if(collisions == h->capacity) break;
    }
    return 0;
}


       
        



    
    

