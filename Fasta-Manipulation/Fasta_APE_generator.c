/* This program parses entries in a fasta to generate interleaved artifical 100bp paired end reads.  It takes a fasta as input. */

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
void createOutputFile (FILE **outFile, char *inName);
void initializeFastaEntry (fastaEntry *newFasta);
void initializeString (string *newString);
void parseRead (FILE *inFile, FILE *outFile);
void readValueToString (string *string, char in);
void reinitializeFastaEntry (fastaEntry *fasta);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 2) {
        printf ("Usage: %s Input_filename\n", argV[0]);
        exit(1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    char in;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Copy the entries
    printf ("Files found and created.  Compiling entries...\n");
    parseRead (inFile, outFile);
//Close everything and free memory
    printf ("  Closing files...\n");
    fclose (inFile);
    fclose (outFile);
    return 0;
}

//Create and check a file opening
void createFile (FILE **file, char *fName, char perm) {
    *file = fopen (fName, &perm);
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
    outTitle = malloc (j + 23);
    outTitle[0] = '\0';
    strcat (outTitle, "Interleaved_");
    strcat (outTitle, fileName);
    strcat (outTitle, "_APE.fastq");
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
void parseRead (FILE *inFile, FILE *outFile) {
//Local variables
    fastaEntry curRead;
    string forSeq, forQual, revSeq, revQual;
    int count = 0, location, readCount = 0, i;
    char in;
//Initialize as needed
    initializeFastaEntry (&curRead);
    in = fgetc (inFile);
//Loop the process
    while (1) {
        location = 0;
        in = fgetc (inFile);
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
//Parse out APEs
        while ((location + 202 ) < curRead.seq.len) {
            i = 0;
//Print forward APE name
            fprintf (outFile, "@%s_%iR1\n", curRead.title.str, ++readCount);
//Print forward APE sequence
            while (i++ < 120) {
                fprintf (outFile, "%c", curRead.seq.str[(location + i)]);
            }
//Print forward quality of PHRED64 32 (`)
            fprintf (outFile, "\n+\n");
            for (i = 0; i < 120; i++) {
                fprintf (outFile, "`");
            }
//Print reverse APE name
            fprintf (outFile, "\n@%s_%iR2\n", curRead.title.str, readCount);
//Print reverse APE sequence
            i = 201;
            while (i-- > 81) {
                fprintf (outFile, "%c", curRead.seq.str[(location + i)]);
            }
//Print reverse quality of PHRED64 32 (`)
            fprintf (outFile, "\n+\n");
            for (i = 0; i < 120; i++) {
                fprintf (outFile, "`");
            }
//Move the frame down the read
            fprintf (outFile, "\n");
            location += 180;
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
    printf ("%d reads parsed into %d APEs.", count, readCount);
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
