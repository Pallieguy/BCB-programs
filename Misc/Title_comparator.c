/* This program finds titles shared between two lists.  It takes any two SM*_title_parser output files as input. */

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
    struct titleEntry *next;
} titleEntry;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void freeTitleEntry (titleEntry *oldEntry);
void freeTitleEntryList (titleEntry *firEntry);
void initializeString (string *newString);
void initializeTitleEntry (titleEntry **newEntry);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s First_title_filename Second_title_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    char in;
    int count = 0;
    FILE *inFile1 = NULL, *inFile2 = NULL, *outFile = NULL;
    string title2;
    titleEntry *firEntry = NULL, *curEntry = NULL, *prevEntry = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile1, argv[1], 'r');
    createFile (&inFile2, argv[2], 'r');
    createFile (&outFile, "Shared_titles.txt", 'w');
//Collect the titles from the inFile1
    printf ("Files opened.  Loading title list...\n");
    initializeTitleEntry (&firEntry);
    curEntry = firEntry;
    while (1) {
        in = fgetc (inFile1);
//Break conditions
        if (((ferror (inFile1)) || (feof (inFile1)))) {
            break;
        }
//Find the title entry and prep the next node
        if (in == '>') {
            while (in != '\t') {
                readValueToString (&curEntry->title, in);
                in = fgetc (inFile1);
            }
            initializeTitleEntry (&curEntry->next);
            curEntry = curEntry->next;
        }
    }
    freeTitleEntry (curEntry);
    fclose (inFile1);
//Search for the titles frome inFile2 in the list
    printf ("Loaded.  Comparing entries...\n");
    initializeString (&title2);
    while (1) {
        in = fgetc (inFile2);
//Break conditions
        if (((ferror (inFile2)) || (feof (inFile2)))) {
            break;
        }
//Load the new title
        if (in == '>') {
            while (in != '\t') {
                readValueToString (&title2, in);
                in = fgetc (inFile2);
            }
//Search the list for it
            curEntry = firEntry;
            while (curEntry != NULL) {
                if (strcmp (curEntry->title.str, title2.str) == 0) {
                    break;
                } else {
                    prevEntry = curEntry;
                    curEntry = curEntry->next;
                }
            }
//If it's found, print and remove the entry from the list
            if (curEntry != NULL) {
                fprintf (outFile, "%s\n", title2.str);
//If it's the first title
                if (curEntry == firEntry) {
                    firEntry = firEntry->next;
//Otherwise
                } else {
                    prevEntry->next = curEntry->next;

                }
                freeTitleEntry (curEntry);
            }
            reinitializeString (&title2);
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0) {
                printf ("%d titles searched...\n", count);
            }
        }
    }
//Close everything and free memory
    printf ("%d titles searched.  Closing files and freeing memory...\n", count);
    fclose (inFile2);
    fclose (outFile);
    printf ("Done.\n");
    return(0);
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

//Frees the memory used by a titleEntry node
void freeTitleEntry (titleEntry *oldEntry) {
    free (oldEntry->title.str);
    free (oldEntry);
    return;
}

//Frees titleEntry list
void freeTitleEntryList (titleEntry *firEntry) {
    titleEntry *curEntry = NULL;
    while (firEntry != NULL) {
        curEntry = firEntry;
        firEntry = firEntry->next;
        freeTitleEntry (curEntry);
    }
    return;
}


//Sets the minimum values to a titleEntry
void initializeTitleEntry (titleEntry **newEntry) {
    *newEntry = malloc (sizeof (**newEntry));
    initializeString (&(*newEntry)->title);
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
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}

//Resets a used string to an empty status
void reinitializeString (string *string) {
    free (string->str);
    initializeString (&(*string));
    return;
}
