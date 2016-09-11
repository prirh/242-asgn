#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <time.h>
#include <string.h>
#include "mylib.h"
#include "htable.h"


/**
 * Prints words and their corresponding frequency of occurance
 * in the hash table.
 * @param freq the frequency of the word.
 * @param word the word to print.
 */
static void print_info(int freq, char *word){
    printf("%-4d %s\n", freq, word);
}


/**
 * Prints the help message.
 */
static void print_help(){
    fprintf(stderr, "Usage: ./htable-asgn [OPTION]... <STDIN> \n");
    fprintf(stderr, "*----------------------------------------------------*\n");
    fprintf(stderr, "Perform various operations using a hash table. \n");
    fprintf(stderr, "By default, words are read from stdin and added to \n");
    fprintf(stderr, "the hash table, before being printed out alongside\n");
    fprintf(stderr, "their frequencies to stdout.\n\n");
    fprintf(stderr, "Arguments:\n");
    fprintf(stderr, " -c FILENAME  Check spelling of words in FILENAME using "
"words\n              from stdin as dictionary. Print unknown words to\n       "
"       stdout, timing info & count to stderr (ignore -p) \n -d           Use "
            "double hashing (linear probing is the default)\n");
    fprintf(stderr," -e           Display entire "
"contents of hash table on stderr\n -p           Print stats info instead"
" of frequencies & words\n -s SNAPSHOTS Show SNAPSHOTS stats snapshots "
"(if -p is used)\n -t TABLESIZE Use the first prime >= TABLESIZE as hash"
" table size\n\n -h           Display this message\n");

}

  
/**
 * Reads in words from a file and stores them in a hash table.
 
 * Perform various operations using a hash table.  By default, words are
 * read from stdin and added to the hash table, before being printed out
 * alongside their frequencies to stdout.
 *
 * -c FILENAME  Check spelling of words in FILENAME using words
 *              from stdin as dictionary.  Print unknown words to
 *              stdout, timing info & count to stderr (ignore -p)
 * -d           Use double hashing (linear probing is the default)
 * -e           Display entire contents of hash table on stderr
 * -p           Print stats info instead of frequencies & words
 * -s SNAPSHOTS Show SNAPSHOTS stats snapshots (if -p is used)
 * -t TABLESIZE Use the first prime >= TABLESIZE as htable size
 * 
 * -h           Display this message
 *
 * @param argc number of command line arguments.
 * @param argv array of the command line arguments.
 * @return EXIT_SUCCESS on execution without errors.
 */
int main(int argc, char **argv){
    /* Timing variables. */
    clock_t start, end;
    double fill = 0.0;
    double search = 0.0;

    /* Hashtable parameters. */
    htable h;
    char word[256];
    int unknown = 0;
    char option;
    FILE *infile;
    hashing_t type = LINEAR_P;
    int cap = 113;
    int snap = 0;

    /* Option variables. */
    int c = 0;
    int e = 0;
    int p = 0;
    int s = 0;

    /* Get options from the command line. */
    const char *optstring = "c:deps:t:h";
    
    /* Switch case for each of the commands. */
    while((option = getopt(argc, argv, optstring)) != EOF){
        switch(option){
            case 'h':
                print_help();
                exit(EXIT_FAILURE);
                break;
            case 'd':
                type = DOUBLE_H;
                break;
            case 'e':
                e = 1;
                break;
            case 's':
                s = atoi(optarg);
                break;
            case 't':
                cap = get_prime(atoi(optarg));
                break;
            case 'c':
                if(NULL == (infile = fopen(optarg, "r"))){
                    fprintf(stderr, "Can't open file! \n");
                    exit(EXIT_FAILURE);
                }
                c = 1;;
                break;
            case 'p':
                p = 1;;
                break;
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }

    h = htable_new(cap, type);
      
    /* Fill hashtable. */
    start = clock();
    while(getword(word, sizeof word, stdin) != EOF){
        htable_insert(h, word);
    }
    end = clock();
    fill = (end-start)/(double)CLOCKS_PER_SEC;


    /* Search file for words in hashtable, print unknowns. */
    if(c == 1){
        start = clock();
        while(getword(word, sizeof word, infile) != EOF){
            if(htable_search(h, word) == 0){
                unknown++;
                printf("%s\n", word);
            }
        }
        end = clock();
        search = (end-start)/(double)CLOCKS_PER_SEC;
        fprintf(stderr, "Fill time:    %.6f\nSearch time:  %.6f\n"
                "Unknown words = %d\n", fill, search, unknown);
        fclose(infile);
        p = 0;
    }

    /* If -e print entire hashtable. */
    if(e == 1){
        htable_print_entire_htable(h);
    }

    /* If -p print stats info unless -c is given. */
    if(p == 1){
        /* If -s option is given, print given number of stats. */
        if(s > 0) snap = s;
        
        htable_print_stats(h, stdout, snap); 
    } else if (c != 1){
        htable_print(h, print_info);   
    }

    /* Free all memory. */
    htable_free(h);

    return EXIT_SUCCESS;
}


    
      
