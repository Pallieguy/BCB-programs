/* This program breaks a fasta into chunks of defined size.  It takes a fasta and an integer as input */

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
void chunkFasta (FILE *inFile, FILE *outFile, int chunkSize);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix);
void initializeString (string *newString);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 3) {
        printf ("Usage: %s Fasta_filename Chunk_size(bp)\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    char *chunk;
    int chunkSize = atoi (argV[2]);
//Parse chunksize into a string
    chunk = malloc (strlen (argV[2]) + 10);
    strcat (chunk, argV[2]);
    strcat (chunk, "_chunked_");
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1], chunk, ".fa");
//Copy the entries
    printf ("Files found and created.  Chunking entries...\n");
    chunkFasta (inFile, outFile, chunkSize);
    fclose (inFile);
    fclose (outFile);
//Close everything and free memory
    printf ("\nDone.\n");
    return 0;
}

//Parse them individually
void chunkFasta (FILE *inFile, FILE *outFile, int chunkSize) {
//Local variables
    char in;
    string title;
    int count = 0, pos;
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
//Print the title
        fprintf (outFile, ">%s_%d-%d\n", title.str, pos + 1, pos + chunkSize);
//Copy sequence
        while (in != '>') {
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            if (in != '\n') {
                fprintf (outFile, "%c", in);
            }
            if (++pos % chunkSize == 0) {
                fprintf (outFile, "\n>%s_%d-%d\n", title.str, pos + 1, pos + chunkSize);
            }
            in = fgetc (inFile);
        }
//Close the outFile and reset the string
        free (title.str);
        title.len = 0;
//A counter so the user has some idea of how long it will take
        if (++count % 10 == 0) {
            printf ("%d entries chunked...\n", count);
        }
    }
    printf ("%d entries chunked.", count);
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

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
