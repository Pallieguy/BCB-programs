/* This program parses entries in a fasta to generate artifical mate pair reads.  It takes a fasta and a number as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Fastq entry components collected into a single struct
typedef struct fastqEntry {
    string *title;
    string *seq;
    string *qual;
    struct fastqEntry *next;
} fastqEntry;

//Functions, in alphabetical order
void copyString (string *dest, string *src);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *coverage);
void freeFastqEntry (fastqEntry *entry);
void initializeFastqEntry (fastqEntry *newFastq);
void initializeString (string *newString);
void invertSequence (string *source);
void parseRead (FILE *inFile, FILE *outFile, int distance);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 3) {
        printf ("Usage: %s Input_filename MP_distance(kbp)\n", argV[0]);
        exit(1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    long distance = 0, length = 0;
    char in;
    distance = atoi (argV[2]);
    distance *= 1000;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1], argV[2]);
//Copy the entries
    printf ("Files found and created.  Compiling entries...\n");
    parseRead (inFile, outFile, distance);
//A counter so the user has some idea of how long it will take
//        if (++count % 10000 == 0) {
//            printf ("%d reads parsed...\n", count);
//        }
//Close everything and free memory
    printf ("  Closing files...\n");
    fclose (inFile);
    fclose (outFile);
    return 0;
}

//Copy a string's content to another string, replacing any content that may have been there before.
void copyString (string *dest, string *src) {
//Local variables
    int i;
    free (dest->str);
//Change size
    dest->len = src->len;
//realloc memory
    dest->str = malloc (dest->len);
//Copy values
    for (i = 0; i < dest->len; i++) {
        dest->str[i] = src->str[i];
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
void createOutputFile (FILE **outFile, char *inName, char *distance) {
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
//Remove the file extention. EDIT THIS AT COPY
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
    outTitle = malloc (strlen (distance) + j + 27);
    outTitle[0] = '\0';
    strcat (outTitle, "Interleaved_");
    strcat (outTitle, fileName);
    strcat (outTitle, "_");
    strcat (outTitle, distance);
    strcat (outTitle, "Kbp_AMP.fastq");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Free a single fasta entry
void freeFastqEntry (fastqEntry *entry) {
    free (entry->title.str);
    free (entry->seq.str);
    free (entry->qual.str);
    free (entry);
    return;
}

//Sets minimum values to a fastq entry
void initializeFastqEntry (fastqEntry *newFastq) {
    initializeString (&newFastq->title);
    initializeString (&newFastq->seq);
    initializeString (&newFastq->qual);
    newFastq->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Inverts the order and nucleotide of a given sequence
void invertSequence (string *sequence) {
//Local variables
    int i = 0, j = (sequence->len - 1);
    string invSequence;
    initializeString (&invSequence);
//Allocate needed memory
    invSequence.len = sequence->len;
    invSequence.str = malloc (invSequence.len);
//Set the null terminator
    invSequence.str[j] = '\0';
    j--;
//Work backwards, inverting as we go
    while (j >= 0) {
        if (sequence->str[i] == 'A') {
            invSequence.str[j] = 'T';
        } else if (sequence->str[i] == 'C') {
            invSequence.str[j] = 'G';
        } else if (sequence->str[i] == 'G') {
            invSequence.str[j] = 'C';
        } else if (sequence->str[i] == 'N') {
            invSequence.str[j] = 'N';
        } else if (sequence->str[i] == 'T') {
            invSequence.str[j] = 'A';
        }
        j--;
        i++;
    }
//Replace the old sequence with the new one
    copyString (sequence, &invSequence);
    free (invSequence.str);
    return;
}

//Goes one read at a time looking for AMPs to parse out of the read and writes them to the outfile
void parseRead (FILE *inFile, FILE *outFile, int distance) {
//Local variables
    fastqEntry curRead, curAMP;
    string forSeq, forQual, revSeq, revQual;
    int count = 0, i;
    char in;
//Initialize as needed
    initializeFastqEntry (&curRead);
    initializeFastqEntry (&curAMP);
//Loop the process
    while (1) {
        in = fgetc (inFile)
        i = 0;
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Load the read, title first
        while (in != '\n') {
            readValueToString (curRead.title, in);
            in = fgetc (inFile);
        }
//Sequence second
        in = fgetc (inFile);
        while (in != '\n') {
            readValueToString (curRead.seq, in);
            in = fgetc (inFile);
        }
//Check that it's long enough
        if (curRead.seq.len > distance) {
//Skip the '+'
            in = fgetc (inFile);
            in = fgetc (inFile);
            in = fgetc (inFile);
//Grab the quality
            while (in != '\n') {
                readValueToString (curRead.qual, in);
                in = fgetc (inFile);
            }
//Parse out AMPs
            while ((i + distance) < curRead.seq.len) {
                
            }
//Otherwise skip to the next entry
        } else {
            in = fgetc (inFile);
            while (in != '\n') {
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
            while (in != '\n') {
                in = fgetc (inFile);
            }
        }
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0) {
            printf ("%d reads parsed...\n", count);
        }
//Clear the read to start the next one
        reinitializeFastqEntryString (&curRead);
        reinitializeFastqEntryString (&curAMP);
    }
//Free everything
    freeFastqEntry (&curRead);
    freeFastqEntry (&curAMP);
return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}

//Reset the strings of a fastqEntry to empty values so it can be reused
void reinitializeFastqEntry (fastqEntry *fastq) {
    fastq->title.len = 1;
    fastq->title.str = realloc (1);
    fastq->title.str[0] = '\0';
    fastq->seq.len = 1;
    fastq->seq.str = realloc (1);
    fastq->seqstr[0] = '\0';
    fastq->qual.len = 1;
    fastq->qual.str = realloc (1);
    fastq->qual.str[0] = '\0';
    return;
}
