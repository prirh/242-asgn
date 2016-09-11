#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "mylib.h"

void *emalloc(size_t s){
  void *p = malloc(s);
  if(NULL == p){
    fprintf(stderr, "Memory allocation failed!\n");
    exit(EXIT_FAILURE);
  }
  return p;
}

void *erealloc(void *p, size_t s){
  p = realloc(p, s);
  if(NULL == p){
    fprintf(stderr, "Memory allocation failed!\n");
    exit(EXIT_FAILURE);
  }
  return p;
}

int getword(char *s, int limit, FILE *stream){
    int c;
    char *w = s;
    assert(limit > 0 && s != NULL && stream != NULL);
    while(!isalnum(c = getc(stream)) && EOF != c);

    if(EOF == c){
        return EOF;
    } else if(--limit > 0){
        *w++ = tolower(c);
    }

    while(--limit > 0){
        if(isalnum(c = getc(stream))){
            *w++ = tolower(c);
        } else if('\'' == c){
            limit++;
        } else break;

    }

    *w = '\0';
    return w - s;
}


/** support function for get_prime */
static int is_prime(int n){
    int i, j=0;
    for(i=1; i<=n; i++){
        if(n%i == 0){
            j++;
        }
    }
    if(j == 2){
        return 1;
    } else if(j > 2){
        return 0;
    }
    return 0;
}

/**
   returns next prime number greater than n
   @param n, number prime should be greater than
   @return i, prime number
**/
int get_prime(unsigned int n){
    int i = n;
    while(1){
        if(is_prime(i)){
            break;
        }
        i++;
    }
    return i;
}

    
    
 
  
