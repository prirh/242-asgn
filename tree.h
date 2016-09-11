#ifndef RBT_H_
#define RBT_H_

typedef struct tree_node *tree;
typedef char type_t;

typedef enum { RED, BLACK } tree_colour;
typedef enum tree_e { BST, RBT } tree_t;

extern tree tree_delete(tree b, char *str);
extern tree tree_free(tree b);
extern tree tree_insert(tree b, char *str);
extern tree tree_new(type_t type);
extern void tree_preorder(tree b, void f(int freq, char *str));
extern void tree_inorder(tree b, void f(int freq, char *str));
extern int tree_search(tree b, char *str);
extern int tree_depth(tree b);
extern void tree_output_dot(tree t, FILE *out);


#endif
