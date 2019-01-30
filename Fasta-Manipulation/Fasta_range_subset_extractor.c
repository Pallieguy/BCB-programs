/* This program parses entries from a fasta within specific provided ranges.  It takes a fasta and a file of titles and ranges to extract as input.  Ranges need to be in the format <fasta entry title>:<start position>-<stop position> */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Fasta data
typedef struct fastaEntry {
    string title;
    string seq;
    struct fastaEntry *next;
} fastaEntry;

//Functions, in alphabetical order
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix);
void initializeFastaEntry (fastaEntry *newEntry);
void initializeString (string *newString);
void loadFastaList (fastaEntry *firFasta, FILE *inFile);
void parseRange (FILE *inFile, fastaEntry *firFasta, char *fileName);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 3) {
        printf ("Usage: %s Fasta_filename Ranges_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *fInFile = NULL, *rInFile = NULL;
    fastaEntry firFasta;
    initializeFastaEntry (&firFasta);
//File creation and checks
    printf ("Opening files...\n");
    createFile (&fInFile, argV[1], 'r');
    createFile (&rInFile, argV[2], 'r');
//Copy the entries
    printf ("Files found and created.  Loading entries...\n");
    initializeFastaEntry (&firFasta);
    loadFastaList (&firFasta, fInFile);
    fclose (fInFile);
    printf ("  Extracting ranges...\n");
    parseRange (rInFile, &firFasta, argV[1]);
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

//Sets minimum values to a fastaEntry
void initializeFastaEntry (fastaEntry *newEntry) {
    initializeString (&(newEntry)->title);
    initializeString (&(newEntry)->seq);
    newEntry->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//loads the fasta entries into a DLL
void loadFastaList (fastaEntry *firFasta, FILE *inFile) {
//Local variables
    char in;
    int count = 0;
    fastaEntry *curFasta = firFasta, *prevFasta = NULL;
    in = fgetc (inFile);
//Skip headers
    while (in != '>') {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            printf ("ERROR: Fasta file improperly formatted!\n");
            exit (3);
        }
        in = fgetc (inFile);
    }
//Loop the rest
    while (1) {
        in = fgetc ( inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }        
//Load the title
        while (in != '\n') {
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                printf ("ERROR: Fasta file improperly formatted!\n");
                exit (3);
            }
            readValueToString (&(curFasta)->title, in);
            in = fgetc (inFile); 
        }
//Load the sequence
        while (in != '>') {
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }        
            in = fgetc (inFile);
            if (in != '\n') {
                readValueToString (&(curFasta)->seq, in);
            }
        }
//Load the next node
        curFasta->next = malloc (sizeof (fastaEntry));
        initializeFastaEntry (curFasta->next);
        prevFasta = curFasta;
        curFasta = curFasta->next;
//A counter so the user has some idea of how long it will take
        if (++count % 100 == 0) {
            printf ("%d fasta entries loaded...\n", count);
        }
    }
//Free last node
    free (curFasta->title.str);
    free (curFasta->seq.str);
    free (curFasta);
    prevFasta->next = NULL;
    printf ("%d fasta entries loaded.", count);
    return;
}

//Extracts the reads as specified
void parseRange (FILE *inFile, fastaEntry *firFasta, char *fileName) {
//Local variables
    int count = 0, start = 0, stop = 0, i, pos, debug = 0;
    char in, *suffix;
    string title;
    fastaEntry *curFasta = NULL;
//Loop it all
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Load the title
        initializeString (&title);
        while (in != ':') {
            readValueToString (&title, in);
            in = fgetc (inFile);
            if (in == '\n') {
                printf ("Ranges must be in format: <fasta entry title>:<start position>-<stop position>\n");
                exit (2);
            }
        }
//Load start and stop
        fscanf (inFile, "%d%*c%d", &start, &stop);
//Skip to the end of the line
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Find the target fasta entry
        curFasta = firFasta;
        while (strcmp (curFasta->title.str, title.str) != 0) {
            curFasta = curFasta->next;
            if (curFasta == NULL) {
                break;
            }
        }
        if (curFasta != NULL) {
//Loop variable
            FILE *outFile = NULL;
//Create the output file
            suffix = malloc (22);
            sprintf (suffix, "_%d-%d.fa", start, stop);
            createOutputFile (&outFile, fileName, NULL, suffix);
//Print the title
            fprintf (outFile, ">%s:%d-%d\n", curFasta->title.str, start, stop);
//Print the sequence
            pos = 0;
            for (i = start; i <= stop; i++) {
                if (curFasta->seq.str[i] == '\0') {
                    break;
                }
                fprintf (outFile, "%c", curFasta->seq.str[i]);
                if (++pos %80 == 0) {
                    fprintf (outFile, "\n");
                }
            }
            fclose (outFile);
        }
//Clear the title
        free (title.str);
        title.len = 0;
//A counter so the user has some idea of how long it will take
        if (++count % 10 == 0) {
            printf ("%d ranges extracted...\n", count);
        }
    }
    printf ("%d ranges extracted.", count);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
