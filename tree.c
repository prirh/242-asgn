#include <stdio.h>
#include <stdlib.h>
#include "mylib.h"
#include "tree.h"
#include <string.h>

#define IS_BLACK(x) ((NULL == (x)) || (BLACK == (x)->colour))
#define IS_RED(x) ((NULL != (x)) && (RED == (x)->colour))

/* holds value passed to tree constructor */
static type_t tree_type;

struct tree_node {
    char *key;
    tree_colour colour;
    tree left;
    tree right;
    int frequency;
};

/**
 * Creates a new null root as the start of the tree and sets tree_type.
 * to the type of tree being created.
 * @param type the type of tree - either RBT or BST.
 * @return null new null space.
 */
tree tree_new(type_t type){
    tree_type = type;
    return NULL; 
}


/**
 * Frees memory associated with given tree.
 * @param b the tree to free.
 * @return b the freed tree.
 */
tree tree_free(tree b){
    if(NULL == b){
        return b;
    }
    
    tree_free(b->left);
    
    tree_free(b->right);
    free(b->key);
    free(b);

    return b;
}

/**
 * Rotates the RBT parent node right.
 * @param r the parent node to rotate on.
 * @return r the resulting parent node after rotation.
 */
static tree right_rotate(tree r){
    tree temp;
    temp = r;
    r = temp->left;
  
    temp->left = r->right;
    r->right = temp;
    return r;
}

/**
 * Rotates the RBT parent node left.
 * @param r the parent node to rotate on.
 * @return r the resulting parent node after rotation.
 */
static tree left_rotate(tree r){
    tree temp;
    temp = r;
    r = temp->right;
 
    temp->right = r->left;
    r->left = temp;
    return r;
}

/**
 * Balances the RBT to satisfy the requirements of a RBT
 * @param r the tree to be fixed
 * @return r the fixed RBT 
 */
static tree rbt_fix(tree r){
    if(IS_RED(r->left) && IS_RED(r->left->left)){
        if(IS_RED(r->right)){
            r->colour = RED;
            r->left->colour = BLACK;
            r->right->colour = BLACK;
        } else {
            r = right_rotate(r);
            r->colour = BLACK;
            r->right->colour = RED;
        }
    } else if(IS_RED(r->left) && IS_RED(r->left->right)){
        if(IS_RED(r->right)){
            r->colour = RED;
            r->left->colour = BLACK;
            r->right->colour = BLACK;
        } else {
            r->left = left_rotate(r->left);
            r = right_rotate(r);
            r->colour = BLACK;
            r->right->colour = RED;
        }
    } else if(IS_RED(r->right) && IS_RED(r->right->left)){
        if(IS_RED(r->left)){
            r->colour = RED;
            r->left->colour = BLACK;
            r->right->colour = BLACK;
        } else {
            r->right = right_rotate(r->right);
            r = left_rotate(r);
            r->colour = BLACK;
            r->left->colour = RED;
        }
    } else if(IS_RED(r->right) && IS_RED(r->right->right)){
        if(IS_RED(r->left)){
            r->colour = RED;
            r->right->colour = BLACK;
            r->left->colour = BLACK;
        } else {
            r = left_rotate(r);
            r->colour = BLACK;
            r->left->colour = RED;
        }
    }
    return r;
}

/**
 * Inserts the key into given tree, if the tree is an RBT then rbt_fix is
 * called after insertion.
 * @param b the tree that the key will be inserted into.
 * @param str the key to add to the tree.
 * @return b the resuling tree after insertion.
 */
tree tree_insert(tree b, char *str){
    
    if(b == NULL){
        b = emalloc(sizeof *b);
        if(tree_type == RBT){
            b->colour = RED;
        }
        b->left = NULL;
        b->right = NULL;
        b->key = NULL;
    }
    if(b->key == NULL){
        b->key = emalloc((strlen(str)+1)*sizeof (char));
        strcpy(b->key, str);
    }
    /* if duplicate item added, increment frequency */
    if(strcmp(b->key, str) == 0){
        b->frequency += 1;
    }
    /* if str is smaller, go left */
    if(strcmp(str, b->key) < 0){
        b->left = tree_insert(b->left, str);
    }
    /* if str is bigger, go right */
    if(strcmp(str, b->key) > 0){
        b->right = tree_insert(b->right, str);
    }
    /* if tree is RBT fix after insert */
    if(tree_type == RBT){
        b = rbt_fix(b);
    }
    return b;
}


/**
 * Applies given function to each key in pre-order traversal.
 * @param b the tree to traverse.
 * @param f the function to be applied to each key.
 * @param str the key to apply function to.
 */
void tree_preorder(tree b, void f(int freq, char *str)){
    if(NULL == b){
        return;
    }
    f(b->frequency, b->key);
    tree_preorder(b->left, f);
    tree_preorder(b->right,f);
}

/**
 * Applied given function to each key in in-order traversal.
 * @param b the tree to traverse.
 * @param f the function to be applied to each key.
 * @param str the key to apply the function to.
 * @param c the colour of the node the key is in - used for RBT only.
 */
void tree_inorder(tree b, void f (int freq, char *str)){
    if(NULL == b){
        return;
    }
    
    tree_inorder(b->left, f);
    f(b->frequency, b->key);
    tree_inorder(b->right,f); 
}

/**
 * Searches tree for given key.
 * @param b the tree to be searched.
 * @param str the key to search for.
 * @return 1 if key is null or 0 if key is found.
 */
int tree_search(tree b, char *str){
    if(b == NULL){
        return 0;
    }
    if(strcmp(str, b->key) == 0){
        return 1;
    }
    if(strcmp(str, b->key) < 0){
        return tree_search(b->left, str);
    }
    if(strcmp(str, b->key) > 0){
        return tree_search(b->right, str);
    }
    return 0;
}

/**
 * Calculates the length of the longest  path between root node
 * and furthest leaf node.
 * @param b the tree to find the depth of.
 * @return depth of the tree, or zero if tree is null.
 */
int tree_depth(tree b){
    if(b == NULL){
        return 0;
    }
    if(b->left == NULL && b->right == NULL) return 0;
    if(tree_depth(b->left) > tree_depth(b->right)){
        return (tree_depth(b->left) + 1);
    }
    return (tree_depth(b->right) + 1);
}

/**
 * Traverses the tree writing a DOT description about connections, and
 * possibly colours, to the given output stream.
 *
 * @param t the tree to output a DOT description of.
 * @param out the stream to write the DOT output to.
 */
static void tree_output_dot_aux(tree t, FILE *out) {
    if(t->key != NULL) {
        fprintf(out, "\"%s\"[label=\"{<f0>%s:%d|{<f1>|<f2>}}\"color=%s];\n",
                t->key, t->key, t->frequency,
                (RBT == tree_type && RED == t->colour) ? "red":"black");
    }
    if(t->left != NULL) {
        tree_output_dot_aux(t->left, out);
        fprintf(out, "\"%s\":f1 -> \"%s\":f0;\n", t->key, t->left->key);
    }
    if(t->right != NULL) {
        tree_output_dot_aux(t->right, out);
        fprintf(out, "\"%s\":f2 -> \"%s\":f0;\n", t->key, t->right->key);
    }
}

/**
 * Output a DOT description of this tree to the given output stream.
 * DOT is a plain text graph description language (see www.graphviz.org).
 * You can create a viewable graph with the command
 *
 *    dot -Tpdf < graphfile.dot > graphfile.pdf
 *
 * You can also use png, ps, jpg, svg... instead of pdf
 *
 * @param t the tree to output the DOT description of.
 * @param out the stream to write the DOT description to.
 */
void tree_output_dot(tree t, FILE *out) {
    fprintf(out, "digraph tree {\nnode [shape = Mrecord, penwidth = 2];\n");
    tree_output_dot_aux(t, out);
    fprintf(out, "}\n");
}
