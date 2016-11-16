/* This program sorts the entries in a csv by a particular column.  It takes a csv as input.  I suspect this will be a one off for the PI440795 Chr08 alignment.*/

/*I wish I could remember how I made this work, this code is repurposed from Fastq_sorter.c and I don't know how I made it work before (I wrote it in the middle of taking a class on C so I was more...adventurous.  This is why/how I learned to comment obsessively, however.  All // comments are added during repurposing, all /* comments are from repurposed code.*/

//Standard includes, Alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Holds data relevant to the fasta entry
typedef struct node {
    string source;
    long sstart;
    long send;
    int ssize;
    char sstrand;
    string destination;
    long dstart;
    long dend;
    int dsize;
    char dstrand;
    struct node* left;
    struct node* right;
    int height;
} node;

//List of functions, alphabetically
void copyString (string *destination, string *source);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
int delete (node *current_node);
static node* double_rotate_with_left (node* k3);
static node* double_rotate_with_right (node* k1);
node* find_min (node *current_node);
static int height (node* current_node);
void initializeString (string *newString);
node* insert (string *source, long sstart, long send, int ssize, char sstrand, string *destination, long dstart, long dend, int dsize, char dstrand, node *current_node);
node* loadEntry (FILE *inFile);
static int max (int l, int r);
void readValueToString (string *string, char in);
void reinitializeString (string *oldString);
static node* single_rotate_with_left (node* k2);
static node* single_rotate_with_right (node* k1);
node* write (node* current_node, FILE *outFile);
int write_ascending (node* current_node, FILE *outFile);

//Global variables (I liked to play fast and loose it seems...)
node *previous_node = NULL, *root_node = NULL;
int write_count = 0;

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Input_filename\n", argv[0]);
        exit (1);
    }
//main variables
    FILE *inFile = NULL, *outFile = NULL;
    node *current_node = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Do everything else?
    printf ("Files found and created.  Compiling entries...\n");
    current_node = loadEntry (inFile);
    fclose (inFile);
//Print the sorted entries in order
    printf ("Writing to file...\n");
    root_node = current_node;
    while (write_ascending (current_node, outFile) != 1);
    printf ("Written.  Closing files...\n");
    fclose (outFile);
    printf ("Done.\n");
    return 0;
}

//Copy the data to one string from another
void copyString (string *destination, string *source) {
//Local variables
    int i;
    for (i = 0; i != source->len; i++) {
        readValueToString (destination, source->str[i]);
    }
    destination->len = source->len;
    return;
}

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
    char *outdestination, *fileName;
    int i = strlen (inName) - 1, j = i;
//Check if there is a path given instead of jsut a filename
    while (i > 0) {
        if (inName[i] == '/') {
            i++;
            break;
        }
        i--;
    }
/*//Remove the file extention.  EDIT THIS AT COPY IF THE EXTENSION ISN'T BEING CHANGED
    while (j-- > 0) {
        if (inName[j] == '.') {
            inName[j] = '\0';
            break;
        }
    } //END EDIT HERE*/
    j = 0;
//Pull the filename from the input argument
    fileName = malloc (strlen (inName) - i + 1);
    fileName[0] = '\0';
    while (inName[i] != '\0') {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outdestination = malloc (j + 7);
    outdestination[0] = '\0';
    strcat (outdestination, "Sorted_");
    strcat (outdestination, fileName);
    free (fileName);
//Create it
    createFile (* (&outFile), outdestination, 'w');
    free (outdestination);
    return;
}

/* Remove a node in the tree. */
int delete (node* current_node) {
    if (current_node == root_node) {
        if (current_node->right == NULL) {
            return 1;
        } else {
        root_node = current_node->right;
        }
    } else if (current_node->right != NULL) {
        previous_node->left = current_node->right;
    } else {
        previous_node->left = NULL;
    }
    return 0;
}

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

/* Find lowest node. */
node* find_min (node* current_node) {
    if (current_node->left == NULL) {
        return current_node;
    } else {
        previous_node = current_node;
        return find_min (current_node->left);
    }
}

/* Get the height of a node. */
static int height (node* current_node) {
    if (current_node == NULL) {
        return -1;
    } else {
        return current_node->height;
    }
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

/* Insert a node into the tree. */
node* insert (string *source, long sstart, long send, int ssize, char sstrand, string *destination, long dstart, long dend, int dsize, char dstrand, node *current_node) {
//If this is the end of the tree, insert a new node
    if (current_node == NULL) {
        /* Create and return a one-node tree */
        current_node = (node*)malloc (sizeof (node));
        initializeString (&current_node->source);
        initializeString (&current_node->destination);
//If allocation fails stop and announce
        if (current_node == NULL) {
            fprintf (stderr, "Out of memory!!! (insert)\n");
            exit (1);
//Otherwise copy the data to the new node
        } else {
            copyString (&current_node->source, * (&source));
            current_node->sstart = sstart;
            current_node->send = send;
            current_node->ssize = ssize;
            current_node->sstrand = sstrand;
            copyString (&current_node->destination, * (&destination));
            current_node->dstart = dstart;
            current_node->dend = dend;
            current_node->dsize = dsize;
            current_node->dstrand = dstrand;
            current_node->height = 0;
            current_node->left = NULL;
            current_node->right = NULL;
        }
//If it's not the end of the tree search through the tree  THIS IS WHERE I CONFUSE MYSELF
//If the current destiantion is larger than current_node
    } else if (strcmp (destination->str, current_node->destination.str) < 0) {
        current_node->left = insert (source, sstart, send, ssize, sstrand, destination, dstart, dend, dsize, dstrand, current_node->left);
//If the tree has wastes nodes reorg by finding the scaffold
        if (height (current_node->left) - height (current_node->right) == 2) {
//Earlier scaffold
            if (strcmp (destination->str, current_node->left->destination.str) < 0) {
                current_node = single_rotate_with_left (current_node);
//Later scaffold
            } else if (strcmp (destination->str, current_node->left->destination.str) > 0) {
                current_node = double_rotate_with_left (current_node);
//Same scaffold
            } else {
//If the current dstart is less than current_node
                if (dstart < current_node->dstart) {
                    current_node->left = insert (source, sstart, send, ssize, sstrand, destination, dstart, dend, dsize, dstrand, current_node->left);
                    if (height (current_node->left) - height (current_node->right) == 2) {
                        if (dstart < current_node->left->dstart) {
                            current_node = single_rotate_with_left (current_node);
                        } else {
                            current_node = double_rotate_with_left (current_node);
                        }
                    } 
//If the current dstart is larger than current_node
                } else if (dstart > current_node->dstart) {
                    current_node->right = insert (source, sstart, send, ssize, sstrand, destination, dstart, dend, dsize, dstrand, current_node->right);
                    if (height (current_node->right) - height (current_node->left) == 2) {
                        if (dstart > current_node->right->dstart) {
                            current_node = single_rotate_with_right (current_node);
                        } else {
                            current_node = double_rotate_with_right (current_node);
                        }
                    }
                }
            }

        }
//If the current destination is smaller than current_node
    } else if (strcmp (destination->str, current_node->destination.str) > 0) {
        current_node->right = insert (source, sstart, send, ssize, sstrand, destination, dstart, dend, dsize, dstrand, current_node->right);
//If the tree has wasted nodes
        if (height (current_node->right) - height (current_node->left) == 2) {
//Later scaffold
            if (strcmp (destination->str, current_node->right->destination.str) > 0) {
                current_node = single_rotate_with_right (current_node);
//Earlier scaffold
            } else if (strcmp (destination->str, current_node->right->destination.str) < 0) {
                current_node = double_rotate_with_right (current_node);
//Same scaffold
            } else {
//If the current dstart is less than current_node
                if (dstart < current_node->dstart) {
                    current_node->left = insert (source, sstart, send, ssize, sstrand, destination, dstart, dend, dsize, dstrand, current_node->left);
                    if (height (current_node->left) - height (current_node->right) == 2) {
                        if (dstart < current_node->left->dstart) {
                            current_node = single_rotate_with_left (current_node);
                        } else {
                            current_node = double_rotate_with_left (current_node);
                        }
                    } 
//If the current dstart is larger than current_node
                } else if (dstart > current_node->dstart) {
                    current_node->right = insert (source, sstart, send, ssize, sstrand, destination, dstart, dend, dsize, dstrand, current_node->right);
                    if (height (current_node->right) - height (current_node->left) == 2) {
                        if (dstart > current_node->right->dstart) {
                            current_node = single_rotate_with_right (current_node);
                        } else {
                            current_node = double_rotate_with_right (current_node);
                        }
                    }
                }
            }
        }
//ADD SUBSORT HERE.  redo the above sort for start position once destinations are the same?
//Once the same target scaffold is found, sort by start position
    } else if (strcmp (destination->str, current_node->destination.str) == 0) {
//If the current dstart is less than current_node
        if (dstart < current_node->dstart) {
            current_node->left = insert (source, sstart, send, ssize, sstrand, destination, dstart, dend, dsize, dstrand, current_node->left);
            if (height (current_node->left) - height (current_node->right) == 2) {
                if (dstart < current_node->left->dstart) {
                    current_node = single_rotate_with_left (current_node);
                } else {
                    current_node = double_rotate_with_left (current_node);
                }
            }
//If the current dstart is larger than current_node
        } else if (dstart > current_node->dstart) {
            current_node->right = insert (source, sstart, send, ssize, sstrand, destination, dstart, dend, dsize, dstrand, current_node->right);
            if (height (current_node->right) - height (current_node->left) == 2) {
                if (dstart > current_node->right->dstart) {
                    current_node = single_rotate_with_right (current_node);
                } else {
                    current_node = double_rotate_with_right (current_node);
                }
            }
        }
    }
    /* Else X is in the tree already; we'll do nothing */
    current_node->height = max (height (current_node->left), height (current_node->right)) + 1;
    return current_node;
}

//Loads all the data into the tree and returns the top node (somehow)
node* loadEntry (FILE *inFile) {
//Local variables
    string source, destination;
    long sstart = 0, send = 0, dstart = 0, dend = 0;
    int ssize = 0, dsize = 0, count = 0;
    char in, sstrand, dstrand;
    in = fgetc (inFile);
    node *current_node = NULL;
    while (1) {
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
        initializeString (&source);
        initializeString (&destination);
//Skip to the next entry
        while (in != '>') {
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Load the entry from the inFile
        while (in != ',') {
            readValueToString (&source, in);
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
        fscanf (inFile, "%ld%*c%ld%*c%d%*c%c", &sstart, &send, &ssize, &sstrand);
        in = fgetc (inFile);
        in = fgetc (inFile);
        while (in != ',') {
            readValueToString (&destination, in);
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
        fscanf (inFile, "%ld%*c%ld%*c%d%*c%c", &dstart, &dend, &dsize, &dstrand);
//Skip the rest of the entry, if any
        while (in != '\n') {
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Insert the entry into the binary tree.  This is the step I don't remember solving :s
        current_node = insert (&source, sstart, send, ssize, sstrand, &destination, dstart, dend, dsize, dstrand, current_node);
//A counter so the user has some idea of time frame.
        if (++count % 1000 == 0) {
            printf ("%d entries sorted...\n", count);
        }
//Reset destination and source strings
        free (source.str);
        free (destination.str);
    }
    printf ("%d entries sorted.  ", count);
    return (current_node);
}

/* Get higher value of two integers. */
static int max (int l, int r) {
    if (l > r) {
        return l;
    } else {
        return r;
    }
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++ (string->len));
    string->str[ ((string->len) - 2)] = in;
    string->str[ ((string->len) - 1)] = '\0';
    return;
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

/* Write data of a node to file. */
node* write (node* current_node, FILE *outFile) {
    fprintf (outFile, "%s,%ld-%ld,%d,%c,%s,%ld-%ld,%d,%c\n", current_node->source.str, current_node->sstart, current_node->send, current_node->ssize, current_node->sstrand, current_node->destination.str, current_node->dstart, current_node->dend, current_node->dsize, current_node->dstrand);
    return current_node;
}

/* Recursively write AVL tree in ascending order */
int write_ascending (node* current_node, FILE *outFile) {
    return delete (write (find_min (current_node), outFile));
}
