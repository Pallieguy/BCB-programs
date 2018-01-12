/* This program parses entries in a fasta to generate interleaved artifical mate pair reads.  It takes a fastq and a number as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Fasta entry components collected into a single struct
typedef struct fastaEntry {
    string title;
    string seq;
    struct fastaEntry *next;
} fastaEntry;

//Functions, in alphabetical order
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *coverage);
void initializeFastaEntry (fastaEntry *newFasta);
void initializeString (string *newString);
void parseRead (FILE *inFile, FILE *outFile, int distance);
void readValueToString (string *string, char in);
void reinitializeFastaEntry (fastaEntry *fasta);

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
//Close everything and free memory
    printf ("  Closing files...\n");
    fclose (inFile);
    fclose (outFile);
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

//Sets minimum values to a fasta entry
void initializeFastaEntry (fastaEntry *newFasta) {
    initializeString (&(*newFasta).title);
    initializeString (&(*newFasta).seq);
    newFasta->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Goes one read at a time looking for AMPs to parse out of the read and writes them to the outfile
void parseRead (FILE *inFile, FILE *outFile, int distance) {
//Local variables
    fastaEntry curRead;
    string forSeq, forQual, revSeq, revQual;
    int count = 0, i, readCount = 0;
    char in;
//Initialize as needed
    initializeFastaEntry (&curRead);
    in = fgetc (inFile);
//Loop the process
    while (1) {
        in = fgetc (inFile);
        i = 0;
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Load the read, title first
        while ((in != '\n') && (in != ' ') && (in != '\t')) {
            readValueToString (&curRead.title, in);
            in = fgetc (inFile);
        }
//Skip extra info
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Sequence second
        in = fgetc (inFile);
        while (in != '>') {
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            } else if (in != '\n') {
                readValueToString (&curRead.seq, in);
            }
            in = fgetc (inFile);
        }
//Check that it's long enough
        if (curRead.seq.len > (distance + 200)) {
//Loop variable
            int j;
//Parse out AMPs
            while ((i + 200 + distance) < curRead.seq.len) {
//Print AMP first name
                fprintf (outFile, "%s_%iR1\n", curRead.title.str, ++readCount);
//First MP is reverse orientation
                j = 99;
                while (j-- >= 0) {
                    fprintf (outFile, "%c", curRead.seq.str[(j + i)]);
                }
//Print AMP second name
                fprintf (outFile, "\n%s_%iR2\n", curRead.title.str, readCount);
//Second MP is forward orientation
                j = 0;
                while (j++ < 100) {
                    fprintf (outFile, "%c", curRead.seq.str[(j + i + distance)]);
                }
//Move the frame down the read
                fprintf (outFile, "\n");
                i += 80;
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
        if (++count % 100000 == 0) {
            printf ("%d reads parsed...\n", count);
        }
//Clear the read to start the next one
        reinitializeFastaEntry (&curRead);
    }
//Free everything
    free (curRead.title.str);
    free (curRead.seq.str);
    printf ("%d reads parsed into %d AMPs.", count, readCount);
return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}

//Reset the strings of a fastaEntry to empty values so it can be reused
void reinitializeFastaEntry (fastaEntry *fasta) {
    fasta->title.len = 1;
    fasta->title.str = realloc (fasta->title.str, 1);
    fasta->title.str[0] = '\0';
    fasta->seq.len = 1;
    fasta->seq.str = realloc (fasta->seq.str, 1);
    fasta->seq.str[0] = '\0';
    return;
}
