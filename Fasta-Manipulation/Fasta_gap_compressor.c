/* This program shorts all N repeats longer than an inputed value to that value.  It takes a fasta and an intiger as input. */

//Important includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *size);
void compressGaps (int maxGapSize, FILE *inFile, FILE *outFile);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Genome_fasta_filename Maximum_gap_size\n", argv[0]);
        exit (1);
    }
//Main variables
    int userSize;
    FILE *inFile = NULL, *outFile = NULL;
//Initialize main variables
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1], argv[2]);
    sscanf (argv[2], "%d", &userSize);
//Build dynamic linked list of scaf data
    printf ("File found.  Compressing gaps...\n");
    compressGaps (userSize, inFile, outFile);
    fclose (inFile);
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
void createOutputFile (FILE **outFile, char *inName, char *size) {
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
    i = strlen (size);
    outTitle = malloc (i + j + 9);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_N");
    strcat (outTitle, size);
    strcat (outTitle, ".fasta");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Read input and gather stats on each entry
void compressGaps (int maxGapSize, FILE *inFile, FILE *outFile) {
//Local variables
    char in;
    long gapLength = 0, scaffLength = 0;
    int count = 0, pos = 0;
//Process the rest
    while (1) {
        in = fgetc (inFile);
        scaffLength++;
//Break conditions
        if ((ferror (inFile)) || (feof(inFile))) {
            break;
//Check if it's a title
        }
        if (in == '>') {
//copy the title to the output
            while (in != '\n') {
                fprintf (outFile, "%c", in);
                in = fgetc (inFile);
            }
            fprintf (outFile, "\n");
//A counter so the user has an estimate of time
            if (++count % 1000 == 0) {
                printf ("%d scaffolds compressed...\n", count);
            }
//Read line until the next title
        } else if (in != '\n') {
            if (in == 'N') {
//If the gap reaches the max, skip the rest
                if (++gapLength == maxGapSize) {
                    while (in == 'N' || in == '\n') {
                        in = fgetc (inFile);
                    }
                }
//Reset gap counter and print the nucleotide
            } else {
                gapLength = 0;
            }
            fprintf (outFile, "%c", in);
//Add newline if needed
            if (++pos % 80 == 0) {
                fprintf (outFile, "\n");
                pos = 0;
            }
        }
    }
    printf ("%d scaffolds compressed.\n", count);
    return;
}
