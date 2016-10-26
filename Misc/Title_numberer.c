/* This program numbers the fasta titles between two title lists.  It takes any two title list files as input.  I suspect this is a one off for the PI440795 contig alignment to Chr08. */

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
    int position;
    struct titleEntry *next;
} titleEntry;

//List of functions, alphabetically
void copyString (string *dest, string *source);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void freeTitleEntry (titleEntry *oldEntry);
void freeTitleEntryList (titleEntry *firEntry);
void initializeString (string *newString);
void initializeTitleEntry (titleEntry **newEntry);
void loadTitleEntryList (titleEntry *firEntry, FILE *inFile);
void numberTitles (titleEntry *firEntry, FILE *inFile, FILE *outFile);
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
    FILE *inFile1 = NULL, *inFile2 = NULL, *outFile1 = NULL, *outFile2 = NULL;
    titleEntry *firEntry = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile1, argv[1], 'r');
    createFile (&inFile2, argv[2], 'r');
    createOutputFile (&outFile1, argv[1]);
    createOutputFile (&outFile2, argv[2]);
//Collect the titles from the inFile1
    printf ("Files opened and created.  Loading title list...\n");
    initializeTitleEntry (&firEntry);
    loadTitleEntryList (firEntry, inFile1);
//Rewind the file pointer and Number the titles in the first list
    rewind (inFile1);
    printf ("Numbering first title set...\n");
    numberTitles (firEntry, inFile1, outFile1);
    fclose (inFile1);
    fclose (outFile1);
//Number the titles in the second list
    printf ("Numbering second title set...\n");
    numberTitles (firEntry, inFile2, outFile2);
    fclose (inFile2);
    fclose (outFile2);
//Close everything and free memory
    printf ("Closing files and freeing memory...\n");
    freeTitleEntryList (firEntry);
    printf ("Done.\n");
    return(0);
}

//Replaces the values of one string with another
void copyString (string *destination, string *source) {
    int i;
    reinitializeString (&(*destination));
    for (i = 0; i < source->len; i++) {
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
    outTitle = malloc (j + 10);
    outTitle[0] = '\0';
    strcat (outTitle, "numbered_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
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

//Loads the first title set and numbers based on those entries
void loadTitleEntryList (titleEntry *firEntry, FILE *inFile) {
//Local Variables
    char in;
    int count = 0;
    titleEntry *curEntry = NULL;
    string title;
    initializeString (&title);
//Loop the whole process
    while (1) {
        reinitializeString (&title);
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Load the title
        while ((in != ',') && (in != ' ') && (in != '\t') && (in != '\n')) {
            readValueToString (&title, in);
            in = fgetc (inFile);
        }
//Burn the rest of the entry line
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Scan the list to see if the title is already there
        curEntry = firEntry;
        while ((curEntry->next != NULL) && (strcmp (title.str, curEntry->title.str) != 0)) {
            curEntry = curEntry->next;
        }
//If it's not then add it
        if (curEntry->next == NULL) {
            copyString (&curEntry->title, &title);
            curEntry->position = ++count;
            initializeTitleEntry (&curEntry->next);
//A counter so the user has some idea of how long it will take
            if (count % 1000 == 0) {
                printf ("%d titles loaded...\n", count);
            }
        }
    }
    printf ("%d titles loaded.  ", count);
    return;
}

//Matches the titles in the inFile to the entry in the list (if there is one), then prints them to the outFile
void numberTitles (titleEntry *firEntry, FILE *inFile, FILE *outFile) {
//Local variables
    char in;
    int count = 0;
    string title;
    titleEntry *curEntry = NULL;
//Loop the whole process
    while (1) {
        reinitializeString (&title);
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Load the title
        while ((in != ',') && (in != ' ') && (in != '\t') && (in != '\n')) {
            readValueToString (&title, in);
            in = fgetc (inFile);
        }
//Burn the rest of the entry line
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Scan the list to see if the title is already there
        curEntry = firEntry;
        while ((curEntry != NULL) && (strcmp (title.str, curEntry->title.str) != 0)) {
            curEntry = curEntry->next;
        }
//Print position, if there is one
        if (curEntry != NULL) {
            fprintf (outFile, "%s\t%d\n", curEntry->title.str, curEntry->position);
        } else {
            fprintf (outFile, "%s\t\n", title.str);
        }
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0) {
            printf ("%d titles numbered...\n", count);
        }
    }
    printf ("%d titles numbered.  ", count);
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
