/* This program takes a fasta and turns it into a fastq with a phred64 score of 29.  It takes a fasta and a gap size number as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *readSize);
void initializeString (string *newString);
void loadSequence (int readSize, int *readCount, int count, FILE *inFile, FILE *outFile);
void readValueToString (string *string, char in);
void reverseSequenceString (string *sequence);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 3) {
        printf ("Usage: %s Input_fasta_filename Gap_size\n", argV[0]);
        exit (1);
    }
//Main variables
    int readCount = 0, count = 1, readSize;
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1], argV[2]);
    readSize = atoi (argV[2]);
//Convert entries
    printf ("Files found and created.  Converting entries...\n");
    while (1) {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Load and print the sequences
        loadSequence (readSize, &readCount, count, inFile, outFile);
        count++;
    }
//Close everything and free memory
    count -= 2;
    printf ("%d reads parsed into %d entries.  Closing files and freeing memory...\n", readCount, count);
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
void createOutputFile (FILE **outFile, char *inName, char *readSize) {
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
    outTitle = malloc (j + 19);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_");
    strcat (outTitle, readSize);
    strcat (outTitle, "bp_AMP.fastq");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Loads the sequence data from inFile in user defined chunks
void loadSequence (int readSize, int *readCount, int count, FILE *inFile, FILE *outFile) {
//Local variables
    char in;
    int i, nCount = 0;
    string sequence, forwardSequence, reverseSequence, quality;
    initializeString (&sequence);
    initializeString (&forwardSequence);
    initializeString (&reverseSequence);
    initializeString (&quality);
//Processes
    in = fgetc (inFile);
//Try again if it's a new line
    while (in == '\n') {
        in = fgetc (inFile);
    }
//Skip new titles
    if (in == '>') {
        while (in != '\n') {
            in = fgetc (inFile);
        }
//But track how many I find
        *readCount += 1;
//A counter so the user has some idea of how long it will take
        if (*readCount % 10000 == 0) {
            printf ("%d reads parsed...\n", *readCount);
        }
//Otherwise rewind for the next read
    } else {
        fseek (inFile, -1, SEEK_CUR);
    }
//Read the first 51 nucleotides
    for (i = 0; i < 51; i++) {
        in = fgetc (inFile);
//Skip newlines
        if (in == '\n') {
            i--;
//Otherwise add the nucleotide to the MP
        } else {
            readValueToString (&forwardSequence, in);
        }
//Reverse the forwardSequence (since MPs are outward)
        reverseSequenceString (&forwardSequence);
    }
//Read through the gap to make sure the read is large enough
    for (i = 51; i < (readSize - 51); i++) {
        in = fgetc (inFile);
//Stop conditions
        if ((ferror (inFile)) || (feof (inFile))) {
            break;
//If it's a new entry rewind one spot for the next loop
        } else if (in == '>') {
            i = 1;
            fseek (inFile, -1, SEEK_CUR);
            break;
//Ignore new lines, but track how many there are
        } else if (in == '\n') {
            i--;
            nCount++;
        }
    }
//If the read covers the gap, get the reverse entry
    if (i == (readSize - 51)) {
        for (i = 0; i < 51; i++) {
            in = fgetc (inFile);
//Stop conditions
            if ((ferror (inFile)) || (feof (inFile))) {
                break;
//If it's a new entry rewind one position to let the loop handle it properly
            } else if (in == '>') {
                reverseSequence.len = 1;
                fseek (inFile, -1, SEEK_CUR);
                break;
//Ignore new lines, but track how many there are
            } else if (in == '\n') {
                i--;
                nCount++;
//Read the nucleotides
            } else if (in != '\n') {
                readValueToString (&reverseSequence, in);
            }
        }
    }
//If the reverse read is full length
    if (reverseSequence.len == 52) {
//Rewind the file pointer toa 20bp shift
        i = ((readSize - 31 + nCount) * -1);
        fseek (inFile, i, SEEK_CUR);
//Create the false quality string
        for (i = 0; i < 51; i++) {
            readValueToString (&quality, ']');
        }
//Print it all
        fprintf (outFile, "@%d\\0\n%s\n+\n%s\n@%d\\1\n%s\n+\n%s\n", count, forwardSequence.str, quality.str, count, reverseSequence.str, quality.str);
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

//Reverses the sequence held in a string
void reverseSequenceString (string *reverseSequence) {
//Local variables,
    int i;
    string tempSequence;
    initializeString (&tempSequence);
//Load tempSequence by reversing the order and nucleotide
    for (i = (reverseSequence->len - 2); i >= 0; i--) {
        if (reverseSequence->str[i] == 'G') {
            readValueToString (&tempSequence, 'C');
        } else if (reverseSequence->str[i] == 'A') {
            readValueToString (&tempSequence, 'T');
        } else if (reverseSequence->str[i] == 'C') {
            readValueToString (&tempSequence, 'G');
        } else if (reverseSequence->str[i] == 'T') {
            readValueToString (&tempSequence, 'A');
        } else {
            readValueToString (&tempSequence, 'N');
        }
    }
//Replace reverseSequence with the contents of tempSequence
    for (i = 0; i < (reverseSequence->len - 1); i++) {
        reverseSequence->str[i] = tempSequence.str[i];
    }
    return;
}
