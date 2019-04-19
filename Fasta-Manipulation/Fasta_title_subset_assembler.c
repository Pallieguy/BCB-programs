/* This program generates a fasta from a source fasta and a subset of fasta titles with start and end points.  It takes a fasta and a title list as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Entry for fasta formatable data
typedef struct fastaEntry {
    string title;
    string sequence;
    struct fastaEntry *next;
} fastaEntry;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void findTitle (fastaEntry *curFasta, FILE *inFile, FILE *outFile);
void initializeFastaEntry (fastaEntry *newFasta);
void initializeString (string *newString);
void loadFastaList (fastaEntry *firFasta, FILE *inFile);
void readValueToString (string *string, char in);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Fasta_filename Read_titles_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *fInFile = NULL, *tInFile = NULL, *outFile = NULL;
    fastaEntry firFasta;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&fInFile, argv[1], 'r');
    createFile (&tInFile, argv[2], 'r');
    createOutputFile (&outFile, argv[2]);
//Build dynamic linked list of scaf data
    printf ("Files found and created.  Compiling scaffold entries...\n");
    initializeFastaEntry (&firFasta);
    loadFastaList (&firFasta, fInFile);
    fclose (fInFile);
//Find and print the titles
    printf ("Finding matches...\n");
    findTitle (&firFasta, tInFile, outFile);
//Close everything
    printf ("Closing files and freeing memory...\n");
    fclose (outFile);
    fclose (tInFile);
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
    while (i < strlen (inName)) {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 3);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, ".fa");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Goes through a text file of fasta titles and finds the corresponding entries
void findTitle (fastaEntry *firFasta, FILE *inFile, FILE *outFile) {
//Local variables
    string title;
    char in;
    int count = 0, start = 0, end = 0, i = 0;
    fastaEntry *curFasta = NULL;
    while (1) {
        curFasta = firFasta;
        in = fgetc (inFile);
        initializeString (&title);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//If it's not a title, jsut copy it
        while (in != '>') {
            fprintf (outFile, "%c", in);
            in = fgetc (inFile);
        }
//Load the title
        while ((in != ' ') && (in != '\t') && (in != '\n')) {
            readValueToString (&title, in);
            in = fgetc (inFile);
        }
//Load the start and end points
        fscanf (inFile, "%d%d", &start, &end);
        in = fgetc (inFile);
//Burn the rest of the line
        while (in != '\n') {
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Find the title in the fastaEntry list
        while (strcmp (title.str, curFasta->title.str) != 0) {
            if (curFasta == NULL) {
                printf ("%s not found in fasta file!\n", title.str);
                exit (1);
            } else {
                curFasta = curFasta->next;
            }
        }
//Print the matching fastaEntry subset and reset the locals
        fprintf (outFile, "%s_%d-%d\n", curFasta->title.str, start--, end--);
        for (start; start < end; start++) {
            fprintf (outFile, "%c", curFasta->sequence.str[start]);
        }
        fprintf (outFile, "\n");
        free (title.str);
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0){
            printf ("%d entries parsed...\n", count);
        }
    }
    printf ("%d entries parsed.  ", count);
    return;
}

//Sets minimum values to a fastaEntry
void initializeFastaEntry (fastaEntry *newFasta) {
    initializeString (&(*newFasta).title);
    initializeString (&(*newFasta).sequence);
    newFasta->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Loads the entries from a fasta file into a linked list
void loadFastaList (fastaEntry *curFasta, FILE *inFile) {
//Local variables
    char in;
    int count = 0;
    in = fgetc (inFile);
    while (1) {
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Read the title
        while ((in != '\n') && (in != '\t') && (in != ' ')) {
            readValueToString (&curFasta->title, in);
            in = fgetc (inFile);
        }
//If there is more than jsut the title, ignore it
        if ((in == '\t') || (in == ' ')) {
            while (in != '\n') {
                in = fgetc (inFile);
            }
        }
//Read the sequence data
        in = fgetc (inFile);
        while (in != '>') {
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            if (in != '\n') {
                readValueToString (&curFasta->sequence, in);
            }
            in = fgetc (inFile);
        }
//Load the next fastaEntry
        if (in == '>') {
            curFasta->next = malloc (sizeof (*curFasta->next));
            initializeFastaEntry (curFasta->next);
            curFasta = curFasta->next;
        }
//A counter so the user has some idea of how long it will take
        if (++count % 10000 == 0){
            printf ("%d fasta entries loaded...\n", count);
        }
    }
    printf ("%d entries loaded.  ", count);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[((string->len) - 2)] = in;
    string->str[((string->len) - 1)] = '\0';
    return;
}
