/* This program makes a contig gff3 from a fasta.  It takes a fasta as input. */

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
void createOutputFile (FILE **outFile, char *inName);
void initializeString (string *newString);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Callsyntax check
    if (argC != 2) {
        printf ("Usage: %s Input_fasta_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    char in;
    string title;
    int sequenceCount = 0, count = 1;
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
    initializeString (&title);
//gff identifier is needed for GMOD apps
    fprintf (outFile, "##gff-version 3\n");
//Read titles and sizes for gff3
    printf ("Files found and created.  Creating gff entries...\n");
//Skip any headers
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
    in = fgetc (inFile);
//First title is read manually
    while ((in != '\n') && (in != ' ') && (in != '\t')) {
        readValueToString (&title, in);
        in = fgetc (inFile);
    }
//Get to the end of the title line
    while (in != '\n') {
        in = fgetc (inFile);
    }
//Automate the rest
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//If it's a new entry, print the old data
        if (in == '>') {
            fprintf (outFile, "%s\t.\tcontig\t1\t%d\t.\t.\t.\tID=%s;Name=%s;\n", title.str, sequenceCount, title.str, title.str);
//Reset the string
            free (title.str);
            initializeString (&title);
            in = fgetc (inFile);
//Load the new title
            while ((in != '\n') && (in != ' ') && (in != '\t')) {
                readValueToString (&title, in);
                in = fgetc (inFile);
            }
//Get to the end of the title line
            while (in != '\n') {
                in = fgetc (inFile);
            }
            sequenceCount = 0;
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0) {
                printf ("%d entries generated...\n", count);
            }
        } else if (in != '\n') {
            sequenceCount++;
        }
    }
    printf ("%d entries generated\n", count);
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
    outTitle = malloc (j + 13);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_Contig.gff3");
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

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
