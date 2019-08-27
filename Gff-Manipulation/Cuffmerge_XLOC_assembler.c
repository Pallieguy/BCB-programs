/* This program parses the XLOCs from a Cufmerge gtf and creates a multi-entry fasta.  It takes a gtf and a fasta file as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    long len;
} string;

//For each entry in the reference fasta
typedef struct fastaEntry {
    string title;
    string sequence;
    struct fastaEntry *next;
} fastaEntry;


//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix);
void freeFastaEntry (fastaEntry *entry);
void freeFastaList (fastaEntry *firFasta);
void initializeFastaEntry (fastaEntry *newEntry);
void initializeString (string *newString);
void loadFastaEntries (fastaEntry *firEntry, FILE *inFile);
void parseXLOCEntries (fastaEntry *firFasta, FILE *inFile, FILE *outFile);
void readValueToString (string *string, char in);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Input_gtf_filename Input_fasta_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *gtfInFile = NULL, *fastaInFile = NULL,  *outFile = NULL;
    fastaEntry firFasta;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&gtfInFile, argv[1], 'r');
    createFile (&fastaInFile, argv[2], 'r');
    createOutputFile (&outFile, argv[1], NULL, ".fa");
    initializeFastaEntry (&firFasta);
//Load the reference fasta
    printf ("Files opened.  Loading Fasta entries...\n");
    loadFastaEntries (&firFasta, fastaInFile);
    fclose (fastaInFile);
//Process the transcripts
    printf ("  Parsing XLOCs...\n");
    parseXLOCEntries (&firFasta, gtfInFile, outFile);
//Close everything and free memory
    printf ("  Freeing memory and closing files...\n");
    freeFastaList (firFasta.next);
    fclose (gtfInFile);
    fclose (outFile);
    printf ("Done.\n");
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
        outTitle = malloc (j + strlen (prefix) + 2);
    } else if (prefix == NULL && suffix != NULL) {
        outTitle = malloc (j + strlen (suffix) + 1);
    } else {
        outTitle = malloc (j + strlen (prefix) + strlen (suffix) + 2);
    }
//Build the filename
    outTitle[0] = '\0';
    if (prefix != NULL) {
        strcat (outTitle, prefix);
        strcat (outTitle, "_");
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

//Frees all data alloc'd for a FastaEntry()
void freeFastaEntry (fastaEntry *entry) {
    free (entry->title.str);
    free (entry->sequence.str);
    free (entry);
    return;
}

//Frees the entire DLL of fastaEntrys
void freeFastaList (fastaEntry *firFasta) {
//Local variables
    fastaEntry *curFasta = firFasta;
    while (firFasta != NULL) {
        curFasta = firFasta->next;
        freeFastaEntry (firFasta);
        firFasta = curFasta;
    }
    return;
}

//Create a new node on a fasta list
void initializeFastaEntry (fastaEntry *newEntry) {
    initializeString (&(newEntry)->title);
    initializeString (&(newEntry)->sequence);
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

//Reads the scaffold names and sequences into memory
void loadFastaEntries (fastaEntry *firFasta, FILE *inFile) {
//Local variables
    char in;
    int count = 1;
    fastaEntry *curFasta = firFasta;
//Skip any headers
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
//Load the first title manually
    in = fgetc (inFile);
    while (in != '\n') {
        readValueToString (&(curFasta)->title, in);
        in = fgetc (inFile);
    }
//Loop the entire process
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//If it's a title
        if (in == '>') {
//Create a new node
            curFasta->next = malloc (sizeof (*curFasta->next));
            initializeFastaEntry (curFasta->next);
            curFasta = curFasta->next;
//Load the title
            in = fgetc (inFile);
            while (in != '\n') {
                readValueToString (&(curFasta)->title, in);
                in = fgetc (inFile);
            }
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0) {
                printf ("%d entries loaded...\n", count);
            }
            in = fgetc (inFile);
//Otherwise, read sequence characters
        } else {
            while (in != '\n') {
                readValueToString (&(curFasta)->sequence, in);
                in = fgetc (inFile);
            }
        }
    }
    printf ("%d entries Loaded.", count);
    return;
}

//Reads the GTF, finds any transcripts, extracts the sequence and prints a new fasta
void parseXLOCEntries (fastaEntry *firFasta, FILE *inFile, FILE *outFile) {
//Local variables
    char in, dir, curDir;
    int count = 0, loc = 1;
    long i = 0, start = 0, end = 0, curStart = 0, curEnd = 0;
    string scaffold, xloc, curScaffold, curXLOC;
    fastaEntry *curFasta = NULL;
//prep the cur* strings
    initializeString (&curScaffold);
    initializeString (&curXLOC);
    in = fgetc (inFile);
//Loop the entire process per line of the GTF
    while (1) {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Prep the variables
        initializeString (&scaffold);
//Read the scaffold
        while (in != '\t') {
            readValueToString (&scaffold, in);
            in = fgetc (inFile);
        }
//Skip to start
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
//Find start and end values
        fscanf (inFile, "%li%li", &start, &end);
//Skip to direction
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
//Load direction
        dir = fgetc (inFile);
//Skip to gene_id
        while (in != '"') {
            in = fgetc (inFile);
        }
//Load gene_id (XLOC)
        in = fgetc (inFile);
        initializeString (&xloc);
        while (in != '"') {
            readValueToString (&xloc, in);
            in = fgetc (inFile);
        }
//Skip to the end of the line
        while (in != '\n') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//If it's the first xloc
        if (curXLOC.len == 1) {
            for (i = 0; i < xloc.len; i++) {
                readValueToString (&curXLOC, xloc.str[i]);
            }
            for (i = 0; i < scaffold.len; i++) {
                readValueToString (&curScaffold, scaffold.str[i]);
            }
            curStart = start;
            curEnd = end;
            curDir = dir;
        }
//If the XLOCs match
        if (strcmp (xloc.str, curXLOC.str) == 0) {
//Direction and scaffold must match
            if (dir != curDir) {
                printf ("%s has strand mismatch!\n", xloc.str);
                exit (2);
            }
            if (strcmp (scaffold.str, curScaffold.str) != 0) {
                printf ("%s has scaffold mismatch!\n", xloc.str);
                exit (2);
            }
//Adjust start and end if needed
            if (curStart > start) {
                curStart = start;
            }
            if (curEnd < end) {
                curEnd = end;
            }
//If the xlocs don't match or the file is done
        }
        if ((strcmp (xloc.str, curXLOC.str) != 0) || (feof (inFile))) {
//Find the corresponding scaffold
            curFasta = firFasta;
            while (strcmp (curFasta->title.str, curScaffold.str) != 0) {
                curFasta = curFasta->next;
//To avoid infinite loops
                if (curFasta == NULL) {
                    printf ("%s not found in reference fasta!\n", curScaffold.str);
                    exit (1);
                }
            }
//Print the title
            fprintf (outFile, ">%s:%li-%li(%c)_%s\n", curScaffold.str, curStart, curEnd, curDir, curXLOC.str);
//Print the found data in fasta format
            loc = 1;
            for (i = (curStart - 1); i < (curEnd - 1); i++) {
                fprintf (outFile, "%c", curFasta->sequence.str[i]);
                if (loc++ % 80 == 0) {
                    fprintf (outFile, "\n");
                }
            }
            fprintf (outFile, "\n");
//Reset strings
            free (curXLOC.str);
            free (curScaffold.str);
            initializeString (&curXLOC);
            initializeString (&curScaffold);
//Update the cur* values
            for (i = 0; i < xloc.len; i++) {
                readValueToString (&curXLOC, xloc.str[i]);
            }
            for (i = 0; i < scaffold.len; i++) {
                readValueToString (&curScaffold, scaffold.str[i]);
            }
            curStart = start;
            curEnd = end;
            curDir = dir;
//A counter so the user has some idea of how long it will take
            if (++count % 10000 == 0) {
                printf ("%d XLOCs parsed...\n", count);
            }
        }
//Reset entry values
        free (scaffold.str);
        free (xloc.str);
        start = 0;
        end = 0;
    }
    printf ("%d XLOCs parsed.", count);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
