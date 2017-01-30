/* This program generates the location named SNPs for Rex.  It takes a blast against a P.vulgaris asssembly and the SNP fasta. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Fasta entry
typedef struct fasta {
    string title;
    string sequence;
    struct fasta *next;
} fasta;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void findSNPs (fasta *firFasta, FILE *inFile, FILE *outFile);
void freeFasta (fasta *entry);
void freeFastaList (fasta *firFasta);
void initializeFasta (fasta *newFasta);
void initializeString (string *newString);
void loadFastaList (fasta *firFasta, FILE *inFile);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 3) {
        printf ("Usage: %s SNP_blast_filename SNP_fasta_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *bInFile = NULL, *fInFile = NULL, *outFile = NULL;
    fasta *firFasta = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&bInFile, argV[1], 'r');
    createFile (&fInFile, argV[2], 'r');
    createOutputFile (&outFile, argV[1]);
//Load fasta entries
    printf ("Loading fasta entries...\n");
    firFasta = malloc (sizeof (*firFasta));
    initializeFasta (firFasta);
    loadFastaList (firFasta, fInFile);
    fclose (fInFile);
//Print SNP referenced sequences
    printf ("  Finding SNP sequences...\n");
    findSNPs (firFasta, bInFile, outFile);
//Close everything and free memory
    printf ("  Closing files and freeing memory...\n");
    fclose (bInFile);
    freeFastaList (firFasta);
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
//Remove the file extention. EDIT THIS AT COPY
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

//Finds the fasta sequences identified by SNP coordinates
void findSNPs (fasta *firFasta, FILE *inFile, FILE *outFile) {
//Local variables
    int count = 0;
    string title, label;
    char in;
    fasta *curFasta = NULL;
//Loop to cover SNP list
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Reset the variables
        curFasta = firFasta;
        initializeString (&label);
        initializeString (&title);
//Load the current SNP match from the BLAST file
//Chromosome
        while (in != '\t') {
            readValueToString (&title, in);
            in = fgetc (inFile);
        }
        readValueToString (&title, '_');
//Start
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&title, in);
            in = fgetc (inFile);
        }
        readValueToString (&title, '-');
//End
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&title, in);
            in = fgetc (inFile);
        }
        readValueToString (&title, '_');
//Strand
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&title, in);
            in = fgetc (inFile);
        }
//Load the original SNP label
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&label, in);
            in = fgetc (inFile);
        }
//Burn the rest of the BLAST line
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Find the corresponding first entry
        readValueToString (&label, '-');
        readValueToString (&label, '1');
        while (strcmp (curFasta->title.str, label.str) != 0) {
            curFasta = curFasta->next;
            if (curFasta == NULL) {
                printf ("%s not found in fasta!\n", label.str);
                exit (2);
            }
        }
//Print it
        fprintf (outFile, ">%s_1\n%s\n", title.str, curFasta->sequence.str);
//Find the corresponding second entry
        label.str[label.len - 2] = '2';
        while (strcmp (curFasta->title.str, label.str) != 0) {
            curFasta = curFasta->next;
            if (curFasta == NULL) {
                printf ("%s not found in fasta!\n", label.str);
                exit (2);
            }
        }
//Print it
        fprintf (outFile, ">%s_2\n%s\n", title.str, curFasta->sequence.str);
//Drop the current SNP data
        free (label.str);
        label.len = 0;
        free (title.str);
        title.len = 0;
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0) {
            printf ("%d entries written...\n", count);
        }
    }
    printf ("%d SNPs written.", count);
    return;
}

//Free a single fasta entry
void freeFasta (fasta *entry) {
    free (entry->title.str);
    free (entry->sequence.str);
    free (entry);
    return;
}

//Free a fasta DLL
void freeFastaList (fasta *firFasta) {
//Local variables
    fasta *curFasta = firFasta->next;
//Easy while loop
    while (curFasta != NULL) {
        freeFasta (firFasta);
        firFasta = curFasta;
        curFasta = curFasta->next;
    }
    return;
}

//Sets minimum values to a fasta entry
void initializeFasta (fasta *newFasta) {
    initializeString (&newFasta->title);
    initializeString (&newFasta->sequence);
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

//Read fasta data from file into a DLL
void loadFastaList (fasta *firFasta, FILE *inFile) {
//Local variables
    int count = 0;
    char in;
    fasta *curFasta = firFasta, *prevFasta = NULL;
//Loop for the whole file
    in = fgetc (inFile);
    while (1) {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//If this isn't the first entry there's somethign wrong with the file
        if (in != '>') {
            printf ("File contains non-entry data!\n");
            exit (2);
        }
//Load the title
        in = fgetc (inFile);
        while (in != '\n') {
            readValueToString (&curFasta->title, in);
            in = fgetc (inFile);
        }
//Load the sequence
        in = fgetc (inFile);
        while (in != '>') {
//In case it reaches the end of the file
            if (feof (inFile)) {
                break;
            } else if (in != '\n') {
                readValueToString (&curFasta->sequence, in);
            }
            in = fgetc (inFile);
        }
//Load a new entry in the fasta DLL
        curFasta->next = malloc (sizeof (*curFasta));
        initializeFasta (curFasta->next);
        prevFasta = curFasta;
        curFasta = curFasta->next;
//A counter so the user has some idea of how long it will take
        if (++count % 500 == 0) {
            printf ("%d entries loaded...\n", count);
        }
    }
    prevFasta->next = NULL;
    freeFasta (curFasta);
    printf ("%d entries loaded.", count);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
