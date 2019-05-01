/* This program takes a fastq and turns it into a fasta.  It takes a fastq as input. */

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
void convert (FILE *inFile, FILE *outFile);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void initializeString (string *newString);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 2) {
        printf ("Usage: %s Input_fastq_filename\n", argV[0]);
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
    convert (inFile, outFile);
//Close everything
    printf ("  Closing files...\n");
    fclose (inFile);
    fclose (outFile);
    return 0;
}

//Write only the applicable info to the outFile
void convert (FILE *inFile, FILE *outFile) {
//Local variables
    char in;
    int count = 0;
//Skip headers
    in = fgetc (inFile);
    while (in != '@') {
        in = fgetc (inFile);
    }
//Loop the process
    while (1) {
        in = fgetc (inFile);
        fprintf (outFile, ">");
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Copy the title
        while (in != '\n') {
            fprintf (outFile, "%c", in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        fprintf (outFile, "\n");
//Copy the sequence
        while (in != '\n') {
            fprintf (outFile, "%c", in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        fprintf (outFile, "\n");
//Skip the rest
        while (in != '\n') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\n') {
            in = fgetc (inFile);
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
        in = fgetc (inFile);
//Easy check to make sure formatting makes sense
        if ((in != '@') && !(feof (inFile))) {
            printf ("Entries must be contained to 4 lines at most!\n");
            exit (3);
        }        
//A counter so the user has some idea of how long it will take
        if (++count % 1000000 == 0) {
            printf ("%d entries reformatted...\n", count);
        }
    }
    printf ("%d titles reformatted.", count);
    return;
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
    strcat (outTitle, ".fasta");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}
