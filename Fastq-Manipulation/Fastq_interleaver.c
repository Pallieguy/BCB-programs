/* This program interleaves two fastqs into a single file.  It takes the shared part of the R1 and R2 files as input.*/

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void freeFastqEntry (fastqEntry *oldEntry);
void initializeFastqEntry (fastqEntry *newEntry);
void initializeString (string *newString);
void loadFastqEntry (fastqEntry *entry, FILE *source);
void readValueToString (string *string, char in);
void reinitializeFastqEntry (fastqEntry *entry);
void reinitializeString (string *string);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Shared_input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    int count = 0;
    FILE *forwardInFile = NULL, *reverseInFile = NULL, *outFile = NULL;
    fastqEntry entry;
//File creation and checks
    printf ("Opening files...\n");
    initializeFastqEntry (&entry);
    entry.title.len = strlen (argv[1]) + 10;
    entry.title.str = realloc (entry.title.str, entry.title.len);
    strcat (entry.title.str, argv[1]);
    strcat (entry.title.str, "_R1.fastq");
    createFile (&forwardInFile, entry.title.str, 'r');
    entry.title.str[(entry.title.len - 8)] = '2';
    createFile (&reverseInFile, entry.title.str, 'r');
    reinitializeString (&entry.title);
    createOutputFile (&outFile, argv[1]);
//Load the entries and print them in an alternatign fashion
    printf ("Files opened.  Interleaving...\n");
//Automate the process
    while (1) {
//Load the entry
        loadFastqEntry (&entry, forwardInFile);
//If there are no entries left stop the loop
        if (entry.title.str == NULL) {
            break;
        }
//Print the entry and reset the entry
        fprintf (outFile, "%s_R1\n%s\n+\n%s\n", entry.title.str, entry.sequence.str, entry.quality.str);
        reinitializeFastqEntry (&entry);
//Load the entry then print it and reset the entry
        loadFastqEntry (&entry, reverseInFile);
        fprintf (outFile, "%s_R2\n%s\n+\n%s\n", entry.title.str, entry.sequence.str, entry.quality.str);
        reinitializeFastqEntry (&entry);
//A counter so the user has some idea of how long it will take
        if (++count % 1000000 == 0) {
            printf ("%d entries interleaved...\n", count);
        }
    }
//Close everything
    printf ("%d entries interleaved.  Closing files and freeing memory...\n", count);
    freeFastqEntry (&entry);
    fclose (forwardInFile);
    fclose (reverseInFile);
    fclose (outFile);
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
    while (inName[i] != '\0') {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 19);
    outTitle[0] = '\0';
    strcat (outTitle, "interleaved_");
    strcat (outTitle, fileName);
    strcat (outTitle, ".fastq");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
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
