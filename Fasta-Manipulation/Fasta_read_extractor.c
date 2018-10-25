/* This program parses entries from a fasta to generate extracts of particular length and overlap from one end or another.  It takes a fasta, "Start" or "End", an extracted length, an overlap size, and number of extracts as inputs. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Functions, in alphabetical order
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix);
void initializeString (string *newString);
void parseRead (FILE *inFile, char *fileName, char dir, int length, int overlap, int xCount);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 6) {
        printf ("Usage: %s Start/End Extract_length(bp) Overlap_size(bp) Extract_count Input_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL;
    char dir;
    int length = 0, overlap = 0, xCount = 0;
//Variable check(s)
    if (strcmp (argV[1], "Start") == 0 || strcmp (argV[1], "start") == 0) {
        dir = 'f';
    } else if (strcmp (argV[1], "End") == 0 || strcmp (argV[1], "end") == 0) {
        dir = 'r';
    } else {
        printf ("Second argument in command must be \"Start\" or \"End\"\n");
        exit (1);
    }
    length = atoi (argV[2]);
    overlap = atoi (argV[3]);
    xCount = atoi (argV[4]);
    printf ("WARNING: If a multi-entry fasta was provided only the first will be used!\n");
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[5], 'r');
//Copy the entries
    printf ("Files found and created.  Extracting entries...\n");
    parseRead (inFile, argV[5], dir, length, overlap, xCount);
//Close everything and free memory
    printf ("Done.\n");
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
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix) {
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
//Remove the file extention.
    while (j-- > 0) {
        if (inName[j] == '.') {
            inName[j] = '\0';
            break;
        }
    }
    j = 0;
//Pull the filename from the input argument
    fileName = malloc (strlen (inName) - i + 1);
    fileName[0] = '\0';
    while (inName[i] != '\0') {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Adjsut title length
    if (prefix != NULL && suffix == NULL) {
        outTitle = malloc (j + strlen (prefix) + 1);
    } else if (prefix == NULL && suffix != NULL) {
        outTitle = malloc (j + strlen (suffix) + 1);
    } else {
        outTitle = malloc (j + strlen (prefix) + strlen (suffix) + 1);
    }
//Build the filename
    outTitle[0] = '\0';
    if (prefix != NULL) {
        strcat (outTitle, prefix);
    }
    strcat (outTitle, fileName);
    if (suffix != NULL) {
        strcat (outTitle, suffix);
    }
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

//Extracts the reads as specified
void parseRead (FILE *inFile, char *fileName, char dir, int length, int overlap, int xCount) {
//Local variables
    string seq;
    int location, sufStart, sufEnd, i;
    char in, *suffix;
//Initialize as needed
    initializeString (&seq);
    printf ("Extracing %d entries...\n", xCount);
//Load the fasta entry
    in = fgetc (inFile);
//Skip to fasta title
    while (in != '>') {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            printf ("ERROR: Fasta file improperly formatted!\n");
            exit (3);
        }
        in = fgetc (inFile);
    }
//Skip the title
    while (in != '\n') {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            printf ("ERROR: Fasta file improperly formatted!\n");
            exit (3);
        }
        in = fgetc (inFile);
    }
//Read the sequence
    in = fgetc (inFile);
    while (in != '>') {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        } else if (in != '\n') {
            readValueToString (&seq, in);
        }
        in = fgetc (inFile);
    }
    fclose (inFile);
//Set the starting point
        if (dir == 'f') {
            location = 0;
        } else {
            location = seq.len - ((length - overlap) * xCount) - overlap - 1;
        }
//Loop the extraction process
    while (xCount > 0) {
//Loop variables
        FILE *outFile = NULL;
//Create the suffix label
        sufStart = location;
        i = 0;
        while (sufStart != 0) {
            sufStart /= 10;
            i++;
        }
        sufStart = i;
        sufEnd = location + length;
        i = 0;
        while (sufEnd != 0) {
            sufEnd /= 10;
            i++;
        }
        sufEnd = i;
        suffix = malloc (sufStart + sufEnd + 6);
        suffix[0] = '\0';
        sprintf (suffix, "_%d-%d.fa", location, location + length);
//Create the output file for this extract
        createOutputFile (&outFile, fileName, NULL, suffix);
//Print the title
        fprintf (outFile, ">%s_%d-%d\n", fileName, location, location + length);
//print the sequence
        i = 0;
        while (i < length) {
            fprintf (outFile, "%c", seq.str[location + i]);
            i++;
            if (i % 80 == 0) {
                fprintf (outFile, "\n");
            }
        }
//Update iterated variables
        xCount--;
        fclose (outFile);
        location += (length - overlap);
//A counter so the user has some idea of how long it will take
        if (xCount % 10 == 0) {
            printf ("%d reads remaining to be extracted...\n", xCount);
        }
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
