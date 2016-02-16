/* This program filters out fastq entries below a certain length.  It takes a fastq and a minimum length as input. */

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
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *minLength);
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
    if (argc != 3) {
        printf ("Usage: %s Input_filename Minimum_length\n", argv[0]);
        exit (1);
    }
//Main varaibles
    int minLength = atoi (argv[2]), count = 0;
    FILE *inFile = NULL, *outFile = NULL;
    fastqEntry entry;
//minLength check
    if (minLength == 0) {
        printf ("Minimum_length must be a numerical value greater than 0.\n");
        exit (3);
    }
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1], argv[2]);
//Check the sequence sizes
    printf ("Files opened.  Removing reads below threshold...\n");
    initializeFastqEntry (&entry);
//Automate the process
    while (1) {
//Load the data
        loadFastqEntry (&entry, inFile);
//Stop the loop if there are no entries left
        if (entry.title.str == NULL) {
            break;
        }
//Printf the data if it's big enough
        if (entry.sequence.len > minLength) {
            printFastqEntry (&entry, outFile);
        }
//Reset the entry
        reinitializeFastqEntry (&entry);
//A counter so the user has some idea of how long it will take
        if (++count % 1000000 == 0) {
            printf ("%d entries processed...\n", count);
        }
    }
//Close everything and free memory
    printf ("%d entries processed.  Closing files...\n", count);
    freeFastqEntry (&entry);
    fclose (inFile);
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
void createOutputFile (FILE **outFile, char *inName, char *minLength) {
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
    outTitle = malloc (j + 6);
    outTitle[0] = '\0';
    strcat (outTitle, "L");
    strcat (outTitle, minLength);
    strcat (outTitle, "_");
    strcat (outTitle, fileName);
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
