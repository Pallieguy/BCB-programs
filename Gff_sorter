/* This program reorders a gff3 file by scaffold.  It takes a gff3 file as input. */
/*HAS ERROR, SECOND ENTRY IN EAHC CHUNK IS FROM NEXT ENTRY CHUNK*/

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
    string entry;
    string scaffold;
    struct node *next;
} node;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void initializeNode (node *newNode);
void initializeString (string *newString);
node* insertNode (node *entry, node *firstNode);
void loadEntry (node *newEntry, FILE *inFile);
void printNodeList (node *firstNode, FILE *outFile);
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
    node *entry = NULL, *firstNode = NULL;
    int count = 0;
    char in;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Load entries into DLL
    printf ("Files opened.  Loading entries...\n");
    while (1) {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
        entry = malloc (sizeof (node));
        initializeNode (entry);
        loadEntry (entry, inFile);
        firstNode = insertNode (entry, firstNode);
        entry = NULL;
//A counter so the user has some idea of how long it will take
        if (++count % 100000 == 0) {
            printf ("%d entries ordered...\n", count);
        }
    }
    fclose (inFile);
//Write ordered list to file
    printf ("%d entries ordered.  Writing to file...\n", count);
    printNodeList (firstNode, outFile);
//Close everything and free memory
    printf ("Writen.  Closing files and freeing memory...\n");
    fclose (outFile);
    return 0;
}

//Copies one string to another string
void copyString (string *src, string *dest) {
//Local variables
    int i;
//If the destination is already being used, free it
    if (dest->len > 1) {
        free (dest->str);
        dest->str = malloc (1);
        dest->str[1] = '\0';
        dest->len = 1;
    }
//Then copy
    for (i = 0; i < (src->len - 1); i++) {
        readValueToString (dest, src->str[i]);
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
    outTitle = malloc (j + 8);
    outTitle[0] = '\0';
    strcat (outTitle, "sorted_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Sets minimum values to a node
void initializeNode (node *newNode) {
    initializeString (&newNode->entry);
    initializeString (&newNode->scaffold);
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

//Inserts a node in the list where it belongs
node* insertNode (node *entry, node *firstNode) {
//Local variables
    node *curNode = firstNode, *prevNode = NULL;
//if the list is empty
    if (firstNode == NULL) {
        firstNode = entry;
//Otherwise, find where the entry belongs
    } else {
        while ((curNode != NULL) && (strcmp (entry->scaffold.str, curNode->scaffold.str) >= 0)) {
            prevNode = curNode;
            curNode = curNode->next;
        }
//If it's the start of the list
        if (curNode == firstNode) {
            entry->next = firstNode;
            firstNode = entry;
//If it's the end of the list
        } else if (curNode == NULL) {
            prevNode->next = entry;
//Otherwise
        } else {
            entry->next = curNode->next;
            curNode->next = entry;
        }
    }
    return firstNode;
}

//Loads data from file to a node
void loadEntry (node *newEntry, FILE *inFile) {
//Local variables
    char in;
    in = fgetc (inFile);
//# denotes a skipable comment line
    while (in == '#') {
        while (in != '\n') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
    }
//Read the scaffold title
    while (in != '\t') {
        readValueToString (&newEntry->scaffold, in);
        in = fgetc (inFile);
    }
//Copy the scaffold title to the entry
    copyString (&newEntry->scaffold, &newEntry->entry);
//Read the rest of the line
    while (in != '\n') {
        readValueToString (&newEntry->entry, in);
        in = fgetc (inFile);
    }
//Move the pointer to the next valid line
    in = fgetc (inFile);
    while (in == '#') {
        while (in != '\n') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
    }
//Rewind if it's not the end of the file
    if (!feof (inFile)) {
        fseek (inFile, -1, SEEK_CUR);
    }
    return;
}

//Prints the nodes
void printNodeList (node *firstNode, FILE *outFile) {
//Local variables
    node *curNode = firstNode;
    string curScaff;
    initializeString (&curScaff);
    copyString (&firstNode->scaffold, &curScaff);
    fprintf (outFile, "##gff-version 3\n");
//One loop
    while (curNode != NULL) {
//If it's a new scaffold print a "###" breaker line
        if (strcmp (curNode->scaffold.str, curScaff.str) != 0) {
            fprintf (outFile, "###\n");
            copyString (&curNode->scaffold, &curScaff);
        }
        fprintf (outFile, "%s\n", curNode->entry.str);
        curNode = curNode->next;
        free (firstNode->entry.str);
        free (firstNode->scaffold.str);
        free (firstNode);
        firstNode = curNode;
    }
    free (curScaff.str);
    fprintf (outFile, "###");
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
