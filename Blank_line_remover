/* This program removes blank lines from a file.  Any file can be given as input */

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
    char in, last;
    FILE *inFile = NULL, *outFile = NULL;
    int count = 0;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Check for leading blank lines
    printf ("Opened.  Removing blank lines...\n");
    in = fgetc (inFile);
    if (in == '\n') {
        while (in == '\n') {
            in = fgetc (inFile);        
        }
    }
    fprintf (outFile, "%c", in);
//Automate the rest
    while (1) {
        in = fgetc (inFile);
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Only skip blank lines
        if (in == '\n') {
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0) {
                printf ("%d lines cleared...\n", count);
            }
            if (last == '\n') {
                while (in == '\n') {
                    in = fgetc (inFile);
//In case of trailing blanks
                    if (feof (inFile)) {
                        break;
                    }
                }
            }
        }
//Otherwise copy and paste
        fprintf (outFile, "%c", in);
        last = in;
    }
//Close everything
    printf ("%d lines cleared.  Closing files...\n", count);
    fclose (inFile);
    fclose (outFile);
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
/*//Remove the file extention EDIT THIS AT COPY
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
    outTitle = malloc (j + 11);
    outTitle[0] = '\0';
    strcat (outTitle, "Blankless_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}
