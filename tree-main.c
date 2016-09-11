#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <time.h>
#include "tree.h"
#include "mylib.h"

static void print_info(int freq, char *word){
    printf("%-4d %s\n", freq, word);
}

static void print_help(){
    fprintf(stderr, "Usage: ./tree-main [OPTION]... <STDIN>\n");
    fprintf(stderr, "\nPerform various operations using a binary tree.  By default, words\n");
    fprintf(stderr, "are read from stdin and added to the tree, before being printed out\n");
    fprintf(stderr, "\n -c FILENAME  Check spelling of words in FILENAME using words\n");
    fprintf(stderr, "              read from stdin as the dictionary.  Print timing\n");
    fprintf(stderr, "              info & unknown words to stderr (ignore -d & -o)\n");
    fprintf(stderr, " -d           Only print the tree depth (ignore -o)\n");
    fprintf(stderr, " -f FILENAME  Write DOT output to FILENAME (if -o given)\n");
    fprintf(stderr, " -o           Output the tree in DOT form to file 'tree-view.dot'\n");
    fprintf(stderr, " -r           Make the tree an RBT (the default is a BST)\n");
    fprintf(stderr, "\n -h           Print this message\n");
}

int main(int argc, char** argv){
    tree t;
    tree_t type = BST;

    clock_t start, end;
    double fill = 0.0;
    double search = 0.0;

    char graphname[200] = "tree-view.dot";
    FILE *infile;
    FILE *outfile = NULL;
    char option;
    char word[256];
    int unknown = 0;

    int c = 0;
    int d = 0;
    int o = 0;

    const char *optstring = "c:df:orh";
    while((option = getopt(argc, argv, optstring)) != EOF){
        switch(option){
            case 'c':
                if(NULL == (infile = fopen(optarg, "r"))){
                    fprintf(stderr, "Can't open file: '%s'\n", optarg);
                    exit(EXIT_FAILURE);
                }
                c = 1;
                break;
            case 'd':
                d = 1;
		break;
            case 'f':
                strcpy(graphname, optarg);
		break;
            case 'o':
                o = 1;
		break;
            case 'r':
                type = RBT;
		break;
            case 'h':
                print_help();
                exit(EXIT_FAILURE);
		break;
            default:
                print_help();
                exit(EXIT_FAILURE);
        }
    }


    /* initialise the tree to a new RBT or BST. */
    t = tree_new(type);

    /* insert items into tree. */
    start = clock();
    while(getword(word, sizeof word, stdin) != EOF){
       t = tree_insert(t, word);
    }
    end = clock();
    fill = (end-start)/(double)CLOCKS_PER_SEC;

    /* Executes if -c is given as an argument. */
    if(c == 1){
        start = clock();
        while(getword(word, sizeof word, infile) != EOF){
            if(tree_search(t, word) == 0){
                unknown++;
                printf("%s\n", word);
            }
        }
        end = clock();
        search = (end-start)/(double)CLOCKS_PER_SEC;
        fprintf(stderr, "Fill time     : %.6f\nSearch time   : %.6f\nUnknown wo\
rds = %d\n", fill, search, unknown);
        fclose(infile);
        d = 0;
        o = 0;
    }

    if(d == 1){
        printf("Depth: %d\n", tree_depth(t));
        o = 0;
    }

    if(o == 1){
        outfile = fopen(graphname, "w");
        tree_output_dot(t, outfile);
        fprintf(stderr, "Creating dot file '%s'\n", graphname); 
        fclose(outfile);
    }

    if(c == 0 && d == 0 && o == 0) tree_preorder(t, print_info);
                
    tree_free(t);

    return EXIT_SUCCESS;
}
