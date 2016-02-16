/* This program takes a fasta and turns it into a fastq with a phred score of 32.  It takes a fasta as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 2) {
        printf ("############### WARNING ###############\nUSE ONLY ON KNOWN QUALITY FASTA phred SCORE IS ASSUMED TO BE AT LEAST 32\nUsage: %s Input_fasta_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    char in;
    int sequenceCount = 0, count = 0;
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Convert entry
    printf ("Files found and created.  Converting entries...\n");
    in = fgetc (inFile);
//Skip any headers
    while (in != '>') {
        in = fgetc (inFile);
    }
//First title has to be done manually
    while (in != '\n') {
        fprintf (outFile, "%c", in);
        in = fgetc (inFile);
    }
    fprintf (outFile, "\n");
//Automate the rest
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//New entry handling
        if (in == '>') {
            fprintf (outFile, "\n+\n");
//Match the sequence to a quality
            while (sequenceCount-- > 0) {
                fprintf (outFile, "A");
            }
            sequenceCount++;
            fprintf (outFile, "\n");
//New title
            while (in != '\n') {
                fprintf (outFile, "%c", in);
                in = fgetc (inFile);
            }
            fprintf (outFile, "\n");
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0) {
                printf ("%d titles reformatted...\n", count);
            }
//Otherwise copy sequence
        } else if (in != '\n') {
            fprintf (outFile, "%c", in);
            sequenceCount++;
        }
    }
//The last entry needs to be manually handled
    fprintf (outFile, "\n+\n");
    while (sequenceCount-- > 0) {
        fprintf (outFile, "A");
    }
    printf ("%d titles reformatted\n", count);
//Close everything
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
    outTitle = malloc (j + 7);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, ".fastq");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}
