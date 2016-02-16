/* This program sorts the entries in a fastq.  It takes a fastq as input. */

//Standard includes, Alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct node {
    char title[110];
    char sequence[110];
    char quality[110];
    struct node*  left;
    struct node*  right;
    int      height;
} node;
 
node* find_min(node *current_node);
static int height(node* current_node);
static int max(int l, int r);
static node* single_rotate_with_left(node* k2);
static node* single_rotate_with_right(node* k1); 
static node* double_rotate_with_left(node* k3);
static node* double_rotate_with_right(node* k1);
node* insert(char title[110], char sequence[110], char quality[110], node *current_node);
int delete(node *current_node);
node* write(node* current_node);
int write_ascending(node* current_node);
int title_compare(char* titlea, char* titleb);

node *previous_node, *root_node;
FILE *out;
int write_count = 0;

int main(int argc, char *argv[]) {
    node *current_node = NULL;
 
if (argc != 3) {
        printf ("Usage: %s Input_filename Output_filename\n", argv[0]);
        exit(1);
    }

    printf ("Opening files...\n");
    FILE *in = fopen (argv[1], "r");
    out = fopen (argv[2], "w");
    if (in == NULL) {
        printf ("Can't open %s\n", argv[1]);
        exit(1);
    }
    if (out == NULL) {
        printf ("Can't create %s\n", argv[2]);
        exit(1);
    }

    printf ("Files found and created.  Compiling entries...\n");
    char title[110], sequence[110], quality[110];
    int count = 0;
    while (fscanf (in, "%s%*[^\n]%*c", title) != EOF) {
        fscanf (in, "%s%*c", sequence);
        fscanf (in, "%*c%*c%s%*c", quality);
        current_node = insert(title, sequence, quality, current_node);
        if (++count % 1000000 == 0) {
            printf ("Entry %d sorted...\n", count);
        }
    }

    printf ("%d entries compiled.  Writing to file...\n", count);
    root_node = current_node;
    while(write_ascending(current_node) != 1);

    printf ("Written.  Closing files...\n");
    fclose (in);
    fclose (out);
    printf ("Done.\n");
    
    return 0;}
 
/* Find lowest node. */
node* find_min(node* current_node) {
    if(current_node->left == NULL) {
        return current_node;
    } else {
        previous_node = current_node;
        return find_min(current_node->left);
    }
}
 
/* Get the height of a node. */
static int height(node* current_node) {
    if(current_node == NULL) {
        return -1;
    } else {
        return current_node->height;
    }
}
 
/* Get higher value of two integers. */
static int max(int l, int r) {
    if(l > r) {
        return l;
    } else {
        return r;
    }
}
 
/* Rotate a node (k2) and its left child 
    note: call single_rotate_with_left only if k2 node has a left child. */
static node* single_rotate_with_left(node* k2) {
    node* k1 = NULL;
 
    k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;
 
    k2->height = max(height(k2->left), height(k2->right)) + 1;
    k1->height = max(height(k1->left), k2->height) + 1;
    return k1; /* new root */
}
 
/* Rotate a node (k1) and its right child
    note: call single_rotate_with_right only if the k1 node has a right child. */
static node* single_rotate_with_right(node* k1) {
    node* k2;
 
    k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;
 
    k1->height = max(height(k1->left), height(k1->right)) + 1;
    k2->height = max(height(k2->right), k1->height) + 1;
 
    return k2;  /* New root */
}
 
/* Left-right double rotation 
    note: call double_rotate_with_left only if k3 node has a left child and k3's left child has a right child. */ 
static node* double_rotate_with_left(node* k3) {
    /* Rotate between k1 and k2 */
    k3->left = single_rotate_with_right(k3->left);
 
    /* Rotate between K3 and k2 */
    return single_rotate_with_left(k3);
}
 
/* Right-left double rotation 
   note: call double_rotate_with_right only if k1 has a right child and k1's right child has a left child. */
static node* double_rotate_with_right(node* k1) {
    /* rotate between K3 and k2 */
    k1->right = single_rotate_with_left(k1->right);
 
    /* rotate between k1 and k2 */
    return single_rotate_with_right(k1);
}
 
/* Insert a new node into the tree. */
node* insert(char title[110], char sequence[110], char quality[110], node *current_node) {
    if(current_node == NULL) {
        /* Create and return a one-node tree */
        current_node = (node*)malloc(sizeof(node));
        if(current_node == NULL) {
            fprintf (stderr, "Out of memory!!! (insert)\n");
            exit(1);
        } else {
            strcpy(current_node->title, title);
            strcpy(current_node->sequence, sequence);
            strcpy(current_node->quality, quality);
            current_node->height = 0;
            current_node->left = current_node->right = NULL;
        }
    } else if(title_compare(title, current_node->title) < 0) {
        current_node->left = insert(title, sequence, quality, current_node->left);
        if(height(current_node->left) - height(current_node->right) == 2) {
            if(title_compare(title, current_node->left->title) < 0) {
                current_node = single_rotate_with_left(current_node);
            } else {
                current_node = double_rotate_with_left(current_node);
            }
        }
    } else if(title_compare(title, current_node->title) > 0) {
        current_node->right = insert(title, sequence, quality, current_node->right);
        if(height(current_node->right) - height(current_node->left) == 2) {
            if(title_compare(title, current_node->right->title) > 0) {
                current_node = single_rotate_with_right(current_node);
            } else {
                current_node = double_rotate_with_right(current_node);
            }
        }
    }
    /* Else X is in the tree already; we'll do nothing */ 
    current_node->height = max(height(current_node->left), height(current_node->right)) + 1;
    return current_node;
}
 
/* Remove a node in the tree. */
int delete(node* current_node) {
    if(current_node == root_node) {
        if(current_node->right == NULL) {
            return 1;
        } else {
        root_node = current_node->right;
        }
    } else if(current_node->right != NULL) {
        previous_node->left = current_node->right;
    } else {
        previous_node->left = NULL;
    }
    return 0;
}
 
/* Write data of a node to file. */
node* write(node* current_node) {
    fprintf (out, "%s\n%s\n+\n%s\n", current_node->title, current_node->sequence, current_node->quality);
    return current_node;
}
 
/* Recursively write AVL tree in ascending order */
int write_ascending(node* current_node) {
    return delete(write(find_min(current_node)));
}

int title_compare(char* titlea, char* titleb) {
    return strcmp(titlea, titleb);
}
