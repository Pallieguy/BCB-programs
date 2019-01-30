/* This program takes a multi-entry fasta and turns it into a series of single entry fastas.  It takes a fasta as input */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Functions, in alphabetical order
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix);
void initializeString (string *newString);
void individualizeFasta (FILE *inFile);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 2) {
        printf ("Usage: %s Fasta_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
//Copy the entries
    printf ("Files found and created.  Individualizing entries...\n");
    individualizeFasta (inFile);
    fclose (inFile);
//Close everything and free memory
    printf ("\nDone.\n");
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

//Parse them individually
void individualizeFasta (FILE *inFile) {
//Local variables
    char in;
    string title;
    int count = 0, pos = 0;
//Skip headers
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            printf ("ERROR: Fasta file improperly formatted!\n");
            exit (2);
        }
    }
//Loop it
    while (1) {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Loop variables
        FILE *outFile = NULL;
        in = fgetc (inFile);
        pos = 0;
//Load title
        initializeString (&title);
        while (in != '\n') {
            readValueToString (&title, in);
            in = fgetc (inFile);
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                printf ("ERROR: Fasta file improperly formatted!\n");
                exit (2);
            }
        }
//Create outfile and print the title
        createOutputFile (&outFile, title.str, NULL, ".fa");
        fprintf (outFile, ">%s\n", title.str);
//Copy sequence
        while (in != '>') {
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            if (in != '\n') {
                fprintf (outFile, "%c", in);
            }
            if (++pos % 80 == 0) {
                fprintf (outFile, "\n");
            }
            in = fgetc (inFile);
        }
//Close the outFile and reset the string
        fclose (outFile);
        free (title.str);
        title.len = 0;
//A counter so the user has some idea of how long it will take
        if (++count % 10 == 0) {
            printf ("%d entries inidividualized...\n", count);
        }
    }
    printf ("%d entries inidividualized.", count);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
