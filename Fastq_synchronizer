/* This program reorders the entries of two fastqs so they are in the same order.  It takes the shared part of the R1 and R2 files as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Standard data from a fastq entry
typedef struct fastqEntry {
    string title;
    string sequence;
    string quality;
    struct fastqEntry *next;
} fastqEntry;

//List of functions, alphabetically
void createFile (FILE **file, char *fname, char perm);
void createOutputFile (FILE **forwardOutFile, FILE **reverseOutFile, FILE **orphanOutFile, char *inName);
void freeFastqEntry (fastqEntry *oldEntry);
void initializeFastqEntry (fastqEntry *newEntry);
void initializeString (string *newString);
void loadFastqEntry (fastqEntry *entry, FILE *source);
void printFastqEntry (fastqEntry *entry, FILE *outFile);
void readValueToString (string *string, char in);
void reinitializeFastqEntry (fastqEntry *entry);
void reinitializeString (string *string);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Shared_input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    int count = 0, match = 0;
    fastqEntry forwardEntry, reverseEntry;
    FILE *forwardInFile = NULL, *reverseInFile = NULL, *forwardOutFile = NULL, *reverseOutFile = NULL, *orphanOutFile;
//File creation and checks
    printf ("Opening files...\n");
    initializeFastqEntry (&forwardEntry);
    initializeFastqEntry (&reverseEntry);
    forwardEntry.title.len = strlen (argv[1]) + 10;
    forwardEntry.title.str = realloc (forwardEntry.title.str, forwardEntry.title.len);
    strcat (forwardEntry.title.str, argv[1]);
    strcat (forwardEntry.title.str, "_R1.fastq");
    createFile (&forwardInFile, forwardEntry.title.str, 'r');
    forwardEntry.title.str[(forwardEntry.title.len - 8)] = '2';
    createFile (&reverseInFile, forwardEntry.title.str, 'r');
    createOutputFile (&forwardOutFile, &reverseOutFile, &orphanOutFile, argv[1]);
    reinitializeFastqEntry (&forwardEntry);
//Check the order of the entries
    printf ("Files opened.  Synchronizing entries...\n");
//Load the first reverse entry outside the loop, since the loop will deal with the forward entry and has an embeded while for the reverse
    loadFastqEntry (&reverseEntry, reverseInFile);
    while (1) {
//Load the forward entry and stop the loop if a list has been completed
        loadFastqEntry (&forwardEntry, forwardInFile);
        if ((forwardEntry.title.str == NULL) || (reverseEntry.title.str == NULL)) {
            break;
        }
//If the reverse entries are 'smaller' than the forward, print them to the orphans until they at least catch up
        while (strcmp (forwardEntry.title.str, reverseEntry.title.str) > 0) {
            printFastqEntry (&reverseEntry, orphanOutFile);
            reinitializeFastqEntry (&reverseEntry);
            loadFastqEntry (&reverseEntry, reverseInFile);
//If the reverse list ends print the forward entry and stop looking
            if (reverseEntry.title.str == NULL) {
                printFastqEntry (&forwardEntry, orphanOutFile);
                break;
            }
        }
//If the entries match, print them and load the next reverse entry
        if (strcmp (forwardEntry.title.str, reverseEntry.title.str) == 0) {
            printFastqEntry (&forwardEntry, forwardOutFile);
            printFastqEntry (&reverseEntry, reverseOutFile);
            reinitializeFastqEntry (&reverseEntry);
            loadFastqEntry (&reverseEntry, reverseInFile);
        } else {
            printFastqEntry (&forwardEntry, orphanOutFile);
        }
        reinitializeFastqEntry (&forwardEntry);
//A counter so the user has some idea of how long it will take
        if (++count % 1000000 == 0) {
            printf ("%d entries sorted.\n", count);
        }
    }
//Check if one list is longer than the other
    printf ("%d entries sorted.  Checking for orphan entries...\n", count);
//If the reverse list was shorter
    count = 0;
    while (forwardEntry.title.str != NULL) {
//Report the orphan, clear the current data and load the next entry
        count++;
        printFastqEntry (&forwardEntry, orphanOutFile);
        reinitializeFastqEntry (&forwardEntry);
        loadFastqEntry (&forwardEntry, forwardInFile);
    }
//If the forward list was shorter
    while (reverseEntry.title.str != NULL) {
//Report the orphan, clear the current data and load the next entry
        count++;
        printFastqEntry (&reverseEntry, orphanOutFile);
        reinitializeFastqEntry (&reverseEntry);
        loadFastqEntry (&reverseEntry, reverseInFile);
    }
//Close everything and free memory
    printf ("%d ophan entries sorted.  Closing files and freeing memory...\n", count);
    freeFastqEntry (&forwardEntry);
    freeFastqEntry (&reverseEntry);
    fclose (forwardInFile);
    fclose (reverseInFile);
    fclose (forwardOutFile);
    fclose (reverseOutFile);
    fclose (orphanOutFile);
    printf ("Done.\n");
    return 0;
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
void createOutputFile (FILE **forwardOutFile, FILE **reverseOutFile, FILE **orphanOutFile, char *inName) {
//Local variables
    char *outTitle, *fileName, *orphanTitle;
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
    while (inName[i] != '\0') {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 18);
    outTitle[0] = '\0';
    orphanTitle = malloc (j + 22);
    orphanTitle[0] = '\0';
    strcat (outTitle, "Synced_");
    strcat (outTitle, fileName);
    strcat (orphanTitle, outTitle);
    strcat (orphanTitle, "_orphans.fastq");
    strcat (outTitle, "_R1.fastq");
    free (fileName);
//Create it
    createFile (*(&forwardOutFile), outTitle, 'w');
    outTitle[(strlen (outTitle) - 7)] = '2';
    createFile (*(&reverseOutFile), outTitle, 'w');
    createFile (*(&orphanOutFile), orphanTitle, 'w');
    free (outTitle);
    free (orphanTitle);
    return;
}

//Frees the memory used by a fastqEntry node
void freeFastqEntry (fastqEntry *oldEntry) {
    free (oldEntry->title.str);
    free (oldEntry->sequence.str);
    free (oldEntry->quality.str);
    free (oldEntry->next);
    return;
}

//Sets the minimum values to a fastqEntry
void initializeFastqEntry (fastqEntry *newEntry) {
    initializeString (&newEntry->title);
    initializeString (&newEntry->sequence);
    initializeString (&newEntry->quality);
    newEntry->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Finds the next entry in a file, sets the string to NULL at EOF
void loadFastqEntry (fastqEntry *entry, FILE *source) {
//Local variables
    char in;
//Find the first entry
    in = fgetc (source);
    while (in != '\n') {
//Set the string to NULL if EOF
        if (((ferror (source)) || (feof (source)))) {
            free (entry->title.str);
            entry->title.str = NULL;
            return;
        }
//Otherwise add it to the title
        readValueToString (&entry->title, in);
        in = fgetc (source);
    }
//Get the sequence
    in = fgetc (source);
    while (in != '\n') {
        readValueToString (&entry->sequence, in);
        in = fgetc (source);
    }
//Skip to the quality line
    fgetc (source);
    fgetc (source);
//Get the quality
    in = fgetc (source);
    while (in != '\n') {
        if (((ferror (source)) || (feof (source)))) {
            break;
        }
        readValueToString (&entry->quality, in);
        in = fgetc (source);
    }
    return;
}

//Prints the contents of a fastqEntry in a controlled format
void printFastqEntry (fastqEntry *entry, FILE *outFile) {
    fprintf (outFile, "%s\n%s\n+\n%s\n", entry->title.str, entry->sequence.str, entry->quality.str);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}

//Resets a used fastqEntry to an empty status
void reinitializeFastqEntry (fastqEntry *entry) {
    reinitializeString (&entry->title);
    reinitializeString (&entry->sequence);
    reinitializeString (&entry->quality);
    return;
}

//Resets a used string to an empty status
void reinitializeString (string *string) {
    free (string->str);
    initializeString (&(*string));
    return;
}
