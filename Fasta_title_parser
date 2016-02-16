/* This Program parses out the titles of each entry in a fasta, it also calculates the length of each entry and a cumulative length of the entries.  It takes a fasta as input. */

//Standard includes
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
        printf ("Usage: %s Input_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    char in;
    int sequenceLength = 0, count = 1;
    long netSequenceLength = 0;
    FILE *inFile, *outFile;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Parse the titles
    printf ("Files found and created.  Parsing scaffold titles...\n");
//Skip any leading lines
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
    in = fgetc (inFile);
//The first one has to be done manually
    while (in != '\n') {
        fprintf (outFile, "%c", in);
        in = fgetc (inFile);
    }
//Automate the rest
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        } 
//New entry resets count
        if (in == '>') {
            fprintf (outFile, "\t%d\t%ld\n", sequenceLength, netSequenceLength);
            in = fgetc (inFile);
            while (in != '\n') {
                fprintf (outFile, "%c", in);
                in = fgetc (inFile);
            }
            sequenceLength = 0;
//A counter so the user has some idea of how long it will take
            if (++count % 10000 == 0) {
                printf ("%d titles parsed...\n", count);
            }
        } else if (in != '\n') {
            sequenceLength++;
            netSequenceLength++;
        }
    }
//Update the last entry manually
    fprintf (outFile, "\t%d\t%ld", sequenceLength,netSequenceLength);
    printf ("%d fasta titles parsed containing %ld bases.\n", count, netSequenceLength);
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
    outTitle = malloc (j + 18);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_Fasta_titles.txt");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}
