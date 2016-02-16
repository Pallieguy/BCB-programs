/* This program removes a specified linker from fastq entry ends, it only keeps sequence before the linker.  It takes a fastq file and a string as input. */

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
void createOutputFile (FILE **outFile, char *inName, char *inLinker);
void freeFastqEntry (fastqEntry *oldEntry);
void initializeFastqEntry (fastqEntry *newEntry);
void initializeString (string *newString);
void loadFastqEntry (fastqEntry *entry, FILE *source);
void printFastqEntry (fastqEntry *entry, FILE *outFile);
void readValueToString (string *string, char in);
void reinitializeFastqEntry (fastqEntry *entry);
void reinitializeString (string *string);
void trimLinker(fastqEntry *entry, char *linker);


//main ()
int main(int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Input_filename Linker_sequence\n", argv[0]);
        exit(1);
    }
//Main variables
    int count = 0;
    FILE *inFile = NULL, *outFile = NULL;
    fastqEntry entry;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1], argv[2]);
//Remove the linker
    printf ("Files opened.  Trimming linker...\n");
//Prep the entry node
    initializeFastqEntry (&entry);
//Automate the process
    while (1) {
//Load the fastq entry
        loadFastqEntry (&entry, inFile);
//Stop the loop if the file is processed
        if (entry.title.str == NULL) {
            break;
        }
//Trim the entry if needed, print it, then reset it
        trimLinker (&entry, argv[2]);
        printFastqEntry (&entry, outFile);
        reinitializeFastqEntry (&entry);
//A counter so the user has some idea of how long it will take
        if (++count % 1000000 == 0) {
            printf ("%d entries processed...\n", count);
        }
    }
//Close everything and free memory
    printf ("%d entries processed.  Closing files and freeing memory...\n", count);
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
void createOutputFile (FILE **outFile, char *inName, char *inLinker) {
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
    i = strlen (inLinker);
    outTitle = malloc (j + i + 10);
    outTitle[0] = '\0';
    strcat (outTitle, inLinker);
    strcat (outTitle, "_trimmed_");
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

//Trim the adaptor and everything following from sequence and corresponding quality
void trimLinker (fastqEntry *entry, char *linker) {
//Local variables
    int sequencePosition, linkerPosition = 0, halfSize = (strlen (linker) / 2), match = 0;
//Start from the left and work right, need to find at least half the linker to call it a match
    for (sequencePosition = 0; sequencePosition < (entry->sequence.len - halfSize); sequencePosition++) {
//If a match is found, state as much
        if (entry->sequence.str[sequencePosition] == linker[linkerPosition]) {
            match = 1;
//Keep looking until at least half the linker is matched
            while ((linkerPosition <= halfSize) && (match == 1)) {
//If the match ends early reset the linker search point and remove the match status
                if (entry->sequence.str[(sequencePosition + linkerPosition)] != linker[linkerPosition]) {
                    match = 0;
                    linkerPosition = 0;
                } else {
                    linkerPosition++;
                }
            }
//If the match lasts for half the linker stop looking
            if (match == 1) {
                break;
            }
        }
    }
//If a match is found, update the new endpoints
    if (match == 1) {
        entry->sequence.len = sequencePosition;
        entry->sequence.str[sequencePosition] = '\0';
        entry->quality.len = sequencePosition;
        entry->quality.str[sequencePosition] = '\0';
    }
    return;
}
