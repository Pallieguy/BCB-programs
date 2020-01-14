/* This program renames the scaffolds LTRharvest produces.  It takes a gff3 file as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//gff entry node
typedef struct node {
    string seq;
    string scaff;
    struct node *next;
} node;

//List of functions, alphabetically
void correctGff (node *firNode, FILE *inFile, FILE *outFile);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void freeNodes (node *firNode);
void initializeNode (node *newNode);
void initializeString (string *newString);
void loadTitles (node *firNode, FILE *inFile);
void printNodeList (node *firNode, FILE *outFile);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 2) {
        printf ("Usage: %s Input_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    node firNode;
    int count = 0;
    char in;
    initializeNode (&firNode);
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Load entries into DLL
    printf ("Files opened.  Loading entries...\n");
    loadTitles (&firNode, inFile);
    printf ("Loaded.  Correcting scaffold names...\n");
    correctGff (&firNode, inFile, outFile);
//Close everything and free memory
    printf ("Writen.  Closing files and freeing memory...\n");
    freeNodes (&firNode);
    fclose (outFile);
    fclose (inFile);
    return 0;
}

void correctGff (node *firNode, FILE *inFile, FILE *outFile) {
//Local variables
    string match;
    int count = 0;
    char in;
    node *curNode = NULL;
    initializeString (&match);
    readValueToString (&match, 's');
    fprintf (outFile, "###gff-version 3\n");
//Loop the process
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Load the match
        while (in != '\t') {
            readValueToString (&match, in);
            in = fgetc (inFile);
        }
//Find it in the node
        curNode = firNode;
        while (strcmp (curNode->seq.str, match.str) != 0) {
            curNode = curNode->next;
        }
//print the right title
        fprintf (outFile, "%s", curNode->scaff.str);
//Copy the rest of the line
        while (in != '\n') {
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            fprintf (outFile, "%c", in);
            in = fgetc (inFile);
        }
//Reset the Match
        fprintf (outFile, "\n");
        free (match.str);
        match.len = 0;
        initializeString (&match);
    }
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
    outTitle = malloc (j + 11);
    outTitle[0] = '\0';
    strcat (outTitle, "corrected_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

void freeNodes (node *firNode) {
    node *curNode = firNode->next;
    while (curNode != NULL) {
        free (firNode->seq.str);
        free (firNode->scaff.str);
        firNode = curNode;
        curNode = curNode->next;
    }
    return;
}

//Sets minimum values to a node
void initializeNode (node *newNode) {
    initializeString (&newNode->seq);
    initializeString (&newNode->scaff);
    newNode->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}


//Loads data from file to a node
void loadTitles (node *firNode, FILE *inFile) {
//Local variables
    char in;
    in = fgetc (inFile);
    node *curNode = firNode;
    int run = 0;
//Skip the tagline
    while (in != '\n') {
        in = fgetc (inFile);
    }
//Loop the rest
    while (1) {
        in = fgetc (inFile);
//Titles all start with '#'
        if (in == '#') {
            in = fgetc (inFile);
//The 'seq' titles all start with "##" though
            if (in == '#') {
//Skip to the title
                while (in != ' ') {
                    in = fgetc (inFile);
                }
                while (in == ' ') {
                    in = fgetc (inFile);
                }
//Load the title
                while (in != ' ') {
                    readValueToString (&curNode->seq, in);
                    in = fgetc (inFile);
                }
//Skip the rest of the string
                while (in != '\n') {
                    in = fgetc (inFile);
                }
//Prep the next node
                curNode->next = malloc (sizeof (*curNode));
                initializeNode (curNode->next);
                curNode = curNode->next;
//First time we reach a scaff title
            } else if (run == 0) {
                run = 1;
                curNode = firNode;
//Load the scaff title
                while (in != '\n') {
                    readValueToString (&curNode->scaff, in);
                    in = fgetc (inFile);
                }
                curNode = curNode->next;
//Every other scaff title
            } else {
                while (in != '\n') {
                    readValueToString (&curNode->scaff, in);
                    in = fgetc (inFile);
                }
                curNode = curNode->next;
            }
        } else {
            break;
        }
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
