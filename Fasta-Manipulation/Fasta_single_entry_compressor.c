/* This program turns a multi-entry fasta into a single entry fasta.  It takes a fasta as input. */

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
        printf ("Usage: %s Input_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    char in;
    FILE *inFile = NULL, *outFile = NULL;
    int count = 0, position = 0;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Compress the entries
    printf ("Files found and created.  Compressing entries...\n");
//Skip any leading lines
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
//The first one has to be done manually
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
        if (in == '>') {
//Keep the fasta at a nice width
            if (position++ == 80) {
                fprintf (outFile, "\n");
                position = 1;
            }
            fprintf (outFile, "-");
            while (in != '\n') {
                in = fgetc (inFile);
            }
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0) {
                printf ("%d entries compressed...\n", count);
            }
//Keep the fasta at a nice width
        } else if (in != '\n') {
            if (position++ == 80) {
                fprintf (outFile, "\n");
                position = 1;
            }
            fprintf (outFile, "%c", in);
        }
    }
    printf ("%d entries compressed...\n", count);
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
    outTitle = malloc (j + 17);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_Single-Entry.fa");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}
