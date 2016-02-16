/* This program separates an interleaved fastq into two matching files.  It takes a fastq as input. */

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
void createOutputFile (FILE **forwardOutFile, FILE **reverseOutFile, char *inName);
void freeFastqEntry (fastqEntry *oldEntry);
void initializeFastqEntry (fastqEntry *newEntry);
void initializeString (string *newString);
void loadFastqEntry (fastqEntry *entry, FILE *source);
void printFastqEntry (fastqEntry *entry, FILE *outFile);
void readValueToString (string *string, char in);
void reinitializeFastqEntry (fastqEntry *entry);
void reinitializeString (string *string);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Interleaved_input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    int count = 0;
    FILE *inFile = NULL, *forwardOutFile = NULL, *reverseOutFile = NULL;
    fastqEntry entry;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&forwardOutFile, &reverseOutFile, argv[1]);
//Split the entries
    printf ("Files opened.  Separating entries...\n");
//Prep the entry node
    initializeFastqEntry (&entry);
//Automate the system
    while (1) {
//Read the first entry
        loadFastqEntry (&entry, inFile);
//Stop the loop if it's empty
        if (entry.title.str == NULL) {
            break;
        }
//Print and clear the data in the entry
        printFastqEntry (&entry, forwardOutFile);
        reinitializeFastqEntry (&entry);
//Rinse and repeat for the reverse
        loadFastqEntry (&entry, inFile);
        printFastqEntry (&entry, reverseOutFile);
        reinitializeFastqEntry (&entry);
//A counter so the user has some idea of how long it will take
        if ((count += 2) % 1000000 == 0) {
            printf ("%d entries separated...\n", count);
        }
    }
//Close everything
    printf ("%d entries separated.  Closing files and freeing memory...\n", count);
    freeFastqEntry (&entry);
    fclose (inFile);
    fclose (reverseOutFile);
    fclose (forwardOutFile);
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
void createOutputFile (FILE **forwardOutFile, FILE **reverseOutFile, char *inName) {
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
//Remove the file extention.  EDIT THIS AT COPY
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
    outTitle = malloc (j + 10);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_R1.fastq");
    free (fileName);
//Create it
    createFile (*(&forwardOutFile), outTitle, 'w');
    outTitle[(strlen (outTitle) - 7)] = '2';
    createFile (*(&reverseOutFile), outTitle, 'w');
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
    char in, in2, in3;
//Find the first entry
    in = fgetc (source);
    while (in != '\n') {
//Set the string to NULL if EOF
        if (((ferror (source)) || (feof (source)))) {
            free (entry->title.str);
            entry->title.str = NULL;
            return;
        }
//Check for the "_R1" or "_R2" identifiers and remove if found
        if (in == '_') {
            in2 = fgetc (source);
            if (in2 == 'R') {
                in3 = fgetc (source);
                if ((in3 == '1') || (in3 == '2')) {
                    in = fgetc (source);
                    if (in == '\n') {
                        break;
                    }
//If they aren't there, reset the file pointer
                } else {
                    fseek (source, -2, SEEK_CUR);
                }
            } else {
                fseek (source, -1, SEEK_CUR);
            }
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
