/* This program filters any duplicate titles from a list.  It takes any *_title_parser output files as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//To store all the titles in a linked manner
typedef struct titleEntry {
    string title;
    string remainder;
    struct titleEntry *next;
} titleEntry;

//List of functions, alphabetically
void copyString (string *source, string *destination);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void freeTitleEntry (titleEntry *oldEntry);
void initializeString (string *newString);
void initializeTitleEntry (titleEntry **newEntry);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Title_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    char in;
    int count = 0;
    FILE *inFile = NULL, *outFile = NULL;
    string title;
    titleEntry *firEntry = NULL, *curEntry = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Collect the titles from the inFile1
    printf ("Files opened.  Loading title list...\n");
    initializeTitleEntry (&firEntry);
    initializeString (&title);
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Find the title entry
        if (in == '>') {
//Load the title
            while ((in != ':') && (in != ' ') && (in != '\t') && (in != '\n')) {
                readValueToString (&title, in);
                in = fgetc (inFile);
            }
//Check if it's in the list
            curEntry = firEntry;
            while (curEntry->title.len != 1) {
                if (strcmp (title.str, curEntry->title.str) == 0) {
                    break;
                }
                curEntry = curEntry->next;
            }
            if (curEntry->title.len == 1) {
                copyString (&title, &curEntry->title);
                while (in != '\n') {
                    readValueToString (&curEntry->remainder, in);
                    in = fgetc (inFile);
                }
                initializeTitleEntry (&curEntry->next);
            }
        reinitializeString (&title);
        }
    }
    fclose (inFile);
//Print out and free the unique title list
    curEntry = firEntry;
    while (curEntry != NULL) {
        fprintf (outFile, "%s%s", curEntry->title.str, curEntry->remainder.str);
        curEntry = curEntry->next;
        if (curEntry != NULL) {
            fprintf (outFile, "\n");
        }
        freeTitleEntry (firEntry);
        firEntry = curEntry;
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0){
            printf ("%d unique titles identified...\n", count);
        }
    }
//Close everything and free memory
    printf ("%d unique titles identified.  Closing files and freeing memory...\n", count);
    fclose (outFile);
    printf ("Done.\n");
    return(0);
}

//Copies the contents of a string from one string to another
void copyString (string *source, string *destination) {
    int i;
    reinitializeString (destination);
    for (i = 0; i < (source->len - 1); i++) {
        readValueToString (destination, source->str[i]);
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
/*//Remove the file extention.  EDIT THIS AT COPY
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
    while (i < strlen (inName)) {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 18);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_unique.txt");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Frees the memory used by a titleEntry node
void freeTitleEntry (titleEntry *oldEntry) {
    free (oldEntry->title.str);
    free (oldEntry->remainder.str);
    free (oldEntry);
    return;
}

//Sets the minimum values to a titleEntry
void initializeTitleEntry (titleEntry **newEntry) {
    *newEntry = malloc (sizeof (**newEntry));
    initializeString (&(*newEntry)->title);
    initializeString (&(*newEntry)->remainder);
    (*newEntry)->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[((string->len) - 2)] = in;
    string->str[((string->len) - 1)] = '\0';
    return;
}

//Resets a used string to an empty status
void reinitializeString (string *string) {
    free (string->str);
    initializeString (&(*string));
    return;
}
