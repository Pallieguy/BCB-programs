/* This program generates a list of titles matched by Mauve. It takes an extensionless Mauve output file as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    char in;
    int count = 0, sequenceCount = 0, start, stop;
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Parse titles
    printf ("Files opened.  Parsing titles...\n");
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Once a title is found
        if (in == '>') {
            fgetc (inFile);
            while (in != ':') {
                fprintf (outFile, "%c", in);
                in = fgetc (inFile);
            }
            fscanf (inFile, "%d%*c%d", &start, &stop);
            sequenceCount = stop - start;
            fgetc (inFile);
            in = fgetc (inFile);
            fprintf (outFile, "\t%d-%d\t%d\t%c\n", start, stop, sequenceCount, in);
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0) {
                printf ("%d titles parsed...\n", count);
            }
//Intermatch seperator character
        } else if (in == '=') {
            fprintf (outFile, "%c\n", in);
        }
    }
//Close everything and free memory
    printf ("%d titles parsed.  Closing files and freeing memory...\n", count);
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
/*//Remove the file extention.  EDIT THIS AT COPY
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
    while (i < strlen (inName)) {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 18);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_Mauve_titles.txt");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}
