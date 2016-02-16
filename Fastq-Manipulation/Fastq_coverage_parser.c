/* This program copies entries in a fastq until a certain Kbp length has been reached.  It takes a fastq and a number as input. */

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
int copyLine (FILE *inFile, FILE *outFile);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *coverage);
void initializeString (string *newString);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 3) {
        printf ("Usage: %s Input_filename Coverage_size(Kbp)\n", argV[0]);
        exit(1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    int count = 0;
    long coverage = 0, length = 0;
    char in;
    coverage = atoi (argV[2]);
    coverage *= 1000;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1], argV[2]);
//Copy the entries
    printf ("Files found and created.  Compiling entries...\n");
    while (length < coverage) {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Copy the lines over
        copyLine (inFile, outFile);
        length+= copyLine (inFile, outFile);
        copyLine (inFile, outFile);
        copyLine (inFile, outFile);
//A counter so the user has some idea of how long it will take
        if (++count % 10000 == 0) {
            printf ("%d reads containing %luKbp parsed...\n", count, length/1000);
        }        
    }
//Check that there was enough reads to actually be usable
    if (length < coverage) {
        printf ("###WARNING###\nOnly %luKbp in provided file\n", length/1000);
    }
//Close everything and free memory
    printf ("%d reads parsed for %lubp.  Closing files...\n", count, length);
    fclose (inFile);
    fclose (outFile);
    return 0;
}

//Copy a single line from inFile to outFile
int copyLine (FILE *inFile, FILE *outFile) {
//Local variables
    char in;
    string entry;
    initializeString (&entry);
//Start with one letter
    in = fgetc (inFile);
//Return if eof or an error in the file
    if (((ferror (inFile)) || (feof (inFile)))) {
        return 0;
    }
//Loop the rest
    while (in != '\n') {
        readValueToString (&entry, in);
        in = fgetc (inFile);
    }
//Print at the end
    fprintf (outFile, "%s\n", entry.str);
    free (entry.str);
//retrun the length copied
    return entry.len;
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
void createOutputFile (FILE **outFile, char *inName, char *coverage) {
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
/*Remove the file extention. EDIT THIS AT COPY
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
    outTitle = malloc (strlen (coverage) + j + 10);
    outTitle[0] = '\0';
    strcat (outTitle, coverage);
    strcat (outTitle, "Kbp_from_");
    strcat (outTitle, fileName);
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
