/* This program sorts the entries in a fastq from largest to smallest.  It takes a fastq as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Node for each entry
typedef struct node {
    string title;
    string sequence;
    string quality;
    struct node* left;
    struct node* right;
    int height;
} node;
 
//These are not my functions, I should figue out how they work some day
static node* double_rotate_with_left (node* k3);
static node* double_rotate_with_right (node* k1);
static int height (node* current_node);
node* insert (node *entry_node, node *current_node);
static int max (int l, int r);
static node* single_rotate_with_left (node* k2);
static node* single_rotate_with_right (node* k1); 
node* write (node* current_node, FILE *outFile);
//These are mine
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
int deleteNode (node *curNode, node *rootNode, node *prevNode);
node* find_leftmost (node *curNode, node *prevNode);
void initializeNode (node *newNode);
void initializeString (string *newString);
void loadEntryNode (node *entryNode, FILE *inFile);
void readValueToString (string *string, char in);
int write_ascending (node* curNode, node* rootNode, node* prevNode, FILE *outFile);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 2) {
        printf ("Usage: %s Input_filename\n", argV[0]);
        exit(1);
    }
//Main variables
    node *curNode = NULL, *entryNode = NULL, *prevNode = NULL, *rootNode = NULL;
    FILE *inFile = NULL, *outFile = NULL;
    int count = 0;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Load the list
    printf ("Files found and created.  Compiling entries...\n");
    while (1) {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Load the next entry from the file
        entryNode = malloc (sizeof (node));
        initializeNode (entryNode);
        loadEntryNode (entryNode, inFile);
//Add it to the tree
        curNode = insert (entryNode, curNode);
        if (++count % 1000000 == 0) {
            printf ("Entry %d sorted...\n", count);
        }
//Clear entry node for the next loop
        entryNode = NULL;
    }
//Print the now sorted list
    printf ("%d entries compiled.  Writing to file...\n", count);
    fclose (inFile);
    rootNode = curNode;
    while (write_ascending(curNode, rootNode, prevNode, outFile) != 1);
//Close everything and free memory
    printf ("Written.  Closing files...\n");
    fclose (outFile);
    return 0;}

/*THESE ARE NOT MINE*/
/* Left-right double rotation 
    note: call double_rotate_with_left only if k3 node has a left child and k3's left child has a right child. */ 
static node* double_rotate_with_left (node* k3) {
    /* Rotate between k1 and k2 */
    k3->left = single_rotate_with_right (k3->left);
    /* Rotate between K3 and k2 */
    return single_rotate_with_left (k3);
}
 
/* Right-left double rotation 
   note: call double_rotate_with_right only if k1 has a right child and k1's right child has a left child. */
static node* double_rotate_with_right (node* k1) {
    /* rotate between K3 and k2 */
    k1->right = single_rotate_with_left (k1->right);
    /* rotate between k1 and k2 */
    return single_rotate_with_right (k1);
}

/* Get the height of a node. */
static int height (node* current_node) {
    if (current_node == NULL) {
        return -1;
    } else {
        return current_node->height;
    }
}

/* Insert a new node into the tree. */
node* insert (node *entry_node, node *current_node) {
    if (current_node == NULL) {
        current_node = entry_node;
    } else if (entry_node->sequence.len - current_node->sequence.len > 0) {
        current_node->left = insert (entry_node, current_node->left);
        if (height (current_node->left) - height (current_node->right) == 2) {
            if (entry_node->sequence.len - current_node->left->sequence.len > 0) {
                current_node = single_rotate_with_left (current_node);
            } else {
                current_node = double_rotate_with_left (current_node);
            }
        }
    } else if (entry_node->sequence.len - current_node->sequence.len <= 0) {
        current_node->right = insert (entry_node, current_node->right);
        if (height (current_node->right) - height (current_node->left) == 2) {
            if (entry_node->sequence.len - current_node->right->sequence.len <= 0) {
                current_node = single_rotate_with_right (current_node);
            } else {
                current_node = double_rotate_with_right (current_node);
            }
        }
    }
    current_node->height = max (height (current_node->left), height (current_node->right)) + 1;
    return current_node;
}

/* Get higher value of two integers. */
static int max (int l, int r) {
    if (l > r) {
        return l;
    } else {
        return r;
    }
}

/* Rotate a node (k2) and its left child 
    note: call single_rotate_with_left only if k2 node has a left child. */
static node* single_rotate_with_left (node* k2) {
    node* k1 = NULL;
    k1 = k2->left;
    k2->left = k1->right;
    k1->right = k2;
    k2->height = max (height (k2->left), height (k2->right)) + 1;
    k1->height = max (height (k1->left), k2->height) + 1;
    return k1; /* new root */
}
 
/* Rotate a node (k1) and its right child
    note: call single_rotate_with_right only if the k1 node has a right child. */
static node* single_rotate_with_right (node* k1) {
    node* k2;
    k2 = k1->right;
    k1->right = k2->left;
    k2->left = k1;
    k1->height = max (height (k1->left), height (k1->right)) + 1;
    k2->height = max (height (k2->right), k1->height) + 1;
    return k2;  /* New root */
}

/* Write data of a node to file */
node* write (node* current_node, FILE *outFile) {
    fprintf (outFile, "%s\n%s\n+\n%s\n", current_node->title.str, current_node->sequence.str, current_node->quality.str);
    return current_node;
}

/*THESE ARE MINE*/
//Create and check a file opening
void createFile (FILE **file, char *fName, char perm) {
    if (perm == 'r') {
        *file = fopen (fName, "r");
    } else if (perm == 'w') {
        *file = fopen (fName, "w");
    } else if (perm == 'a') {
        *file = fopen (fName, "a");
    }
//Check that fopen worked
    if (*file == NULL) {
        printf ("Can't access %s\n", fName);
        exit (2);
    }
    return;
}

//Generates an output file named after the input file.
void createOutputFile (FILE **outFile, char *inName) {
//Local variables
    char *outTitle, *fileName;
    int i = strlen (inName) - 1, j = i;
//Check if there is a path given instead of jsut a filename
    while (i > 0) {
        if (inName[i] == '/') {
            i++;
            break;
        }
        i--;
    }
/*Remove the file extention. EDIT THIS AT COPY
    while (j-- > 0) {
        if (inName[j] == '.') {
            inName[j] = '\0';
            break;
        }
    }//END EDITS HERE*/
    j = 0;
//Pull the filename from the input argument
    fileName = malloc (strlen (inName) - i + 1);
    fileName[0] = '\0';
    while (inName[i] != '\0') {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 13);
    outTitle[0] = '\0';
    strcat (outTitle, "size_sorted_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Remove a node in the tree, return a 1 if the tree is empty, 0 if it's not
int deleteNode (node *curNode, node *rootNode, node *prevNode) {
//If it's the root node, adjsut the root position
    if (curNode == rootNode) {
//Otherwise return the loop kill value
        if (curNode->right == NULL) {
            return 1;
        } else {
        rootNode = curNode->right;
        }
//If there is a right node, then have the previous node point to it
    } else if (curNode->right != NULL) {
        prevNode->left = curNode->right;
//Otherwise, jsut delete it
    } else {
        prevNode->left = NULL;
    }
//Free the memory of curNode
    free (curNode->title.str);
    free (curNode->sequence.str);
    free (curNode->quality.str);
    return 0;
}

//Find leftmost node
node* find_leftmost (node *curNode, node *prevNode) {
    if (curNode->left == NULL) {
        return curNode;
    } else {
        prevNode = curNode;
        return find_leftmost (curNode->left, prevNode);
    }
}

//(Re)Sets a node to empty values
void initializeNode (node *newNode) {
    initializeString (&newNode->title);
    initializeString (&newNode->sequence);
    initializeString (&newNode->quality);
    newNode->left = NULL;
    newNode->right = NULL;
    newNode->height = -1;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Read the entry data from file
void loadEntryNode (node *entryNode, FILE *inFile) {
//Local variables
    char in;
//Load the title
    in = fgetc (inFile);
    while (in != '\n') {
        readValueToString (&entryNode->title, in);
    }
//Load the sequence
    in = fgetc (inFile);
    while (in != '\n') {
        readValueToString (&entryNode->sequence, in);
    }
//Skip the '+'
    in = fgetc (inFile);
    in = fgetc (inFile);
//Load the quality
    in = fgetc (inFile);
    while (in != '\n') {
        readValueToString (&entryNode->quality, in);
    }
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}

//Recursively write the tree in ascending order
int write_ascending (node* curNode, node* rootNode, node* prevNode, FILE *outFile) {
    return deleteNode (write (find_leftmost (curNode, prevNode), outFile), rootNode, prevNode);
}
