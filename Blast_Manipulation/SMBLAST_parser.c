/* This program Parses out a BLAST output based on provided parameters.  It takes a tabular BLAST output file, a column number, and a match parameter as input */

//Standard includes, alphabetically
#include <ctype.h>
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

//main()
int main (int argc, char *argv[]) {
//Call syntax checks
    if (argc != 4) {
        printf ("Usage: %s BLAST_output_filename Column_number match_parameter\n", argv[0]);
        exit (1);
    } else if (!isdigit (argv[2][0])) {
        printf ("Usage: %s BLAST_output_filename Column_number match_parameter\nColumn_number must be a number\n", 
argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    string entry, match;
    int tarCol = atoi (argv[2]), curCol = 1, i = 0, count = 0;
    char in;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Loop it all
    while (1) {
        initializeString (&entry);
        initializeString (&match);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Read the entry
        in = getc (inFile);
        while (in != '\n') {
            readValueToString (&entry, in);
            in = getc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Parse the entry to the correct column
        while (curCol < tarCol) {
            while (in != '\t') {
                in = entry.str[i++];
            }
            in = entry.str[i++];
            curCol++;
        }
//Parse column value
        while (in != '\t') {
            readValueToString (&match, in);
            in = entry.str[i++];
        }
//If the value at column X of entry is the same as the match parameter, print the entry
        if (strcmp (match.str, argv[3]) == 0) {
            fprintf (outFile, "%s\n", entry.str);
        }
//Reset variables
        i = 0;
        curCol = 1;
        free (entry.str);
        entry.len = 0;
        free (match.str);
        match.len = 0;
//A counter so the user has some idea of time frame.
        if (++count % 10000 == 0) {
            printf ("Entry %d compared...\n", count);
        }
    }
//Close everything
    printf ("%d entries compared.  Closing files and freeing memory...\n", count);
    fclose (outFile);
    fclose (inFile);
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
    outTitle = malloc (j + 7);
    outTitle[0] = '\0';
    strcat (outTitle, "parsed_");
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
    string->str[((string->len) - 2)] = in;
    string->str[((string->len) - 1)] = '\0';
    return;
}
