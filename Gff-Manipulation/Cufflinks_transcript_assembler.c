/* This program parses the transcripts from a Cufflinks gtf and creates a multi-entry fasta.  It takes a gtf and a fasta file as input. */

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
void createOutputFile (FILE **outFile, char *inName);
void createFastaEntry (fastaEntry **newEntry);
void freeFastaEntry (fastaEntry **entry);
void initializeString (string *newString);
void loadFastaEntries (fastaEntry *firEntry, FILE *inFile);
void parseTranscriptEntries (fastaEntry *firFasta, FILE *inFile, FILE *outFile);
void readValueToString (string *string, char in);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Input_gtf_filename Input_fasta_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inGFile = NULL, *inFFile = NULL,  *outFile = NULL;
    fastaEntry *firEntry = NULL, *curEntry = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inGFile, argv[1], 'r');
    createFile (&inFFile, argv[2], 'r');
    createOutputFile (&outFile, argv[1]);
    createFastaEntry (&firEntry);
//Load the reference fasta
    printf ("Files opened.  Loading Fasta entries...\n");
    loadFastaEntries (firEntry, inFFile);
    fclose (inFFile);
//Process the transcripts
    printf ("  Parsing transcripts...\n");
    parseTranscriptEntries (firEntry, inGFile, outFile);
//Close everything and free memory
    printf ("  Freeing memory and closing files...\n");
    while (firEntry != NULL) {
        curEntry = firEntry->next;
        freeFastaEntry (&firEntry);
        firEntry = curEntry;
    }
    fclose (inGFile);
    fclose (outFile);
    printf ("Done.\n");
    return 0;
}

//Create a new node on a fasta list
void createFastaEntry (fastaEntry **newEntry) {
    *newEntry = malloc (sizeof (**newEntry));
    initializeString (&(*newEntry)->title);
    initializeString (&(*newEntry)->sequence);
    (*newEntry)->next = NULL;
    return;
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

//Frees all data alloc'd by createFastaEntry()
void freeFastaEntry (fastaEntry **entry) {
    free ((*entry)->title.str);
    free ((*entry)->sequence.str);
    free (*entry);
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
void loadFastaEntries (fastaEntry *firEntry, FILE *inFile) {
//Local variables
    char in;
    int count = 0;
    fastaEntry *curEntry = firEntry;
//Skip any headers
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
//Load the first title manually
    in = fgetc (inFile);
    while (in != '\n') {
        readValueToString (&(curEntry)->title, in);
        in = fgetc (inFile);
    }
    count++;
//Loop the entire process
    while (1) {
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Skip newlines
        if  (in == '\n') {
            in = fgetc (inFile);
//If it's a title
        }
        if (in == '>') {
//Create a new node
            createFastaEntry (&(curEntry->next));
            curEntry = curEntry->next;
//Load the title
            in = fgetc (inFile);
            while (in != '\n') {
                readValueToString (&(curEntry)->title, in);
                in = fgetc (inFile);
            }
//A counter so the user has some idea of how long it will take
            if (++count % 10000 == 0) {
                printf ("%d entries loaded...\n", count);
            }
            in = fgetc (inFile);
//Otherwise, read sequence characters
        } else {
            readValueToString (&(curEntry)->sequence, in);
        }
        in = fgetc (inFile);
    }
    printf ("%d entries Loaded.", count);
    return;
}

//Reads the GTF, finds any transcripts, extracts the sequence and prints a new fasta
void parseTranscriptEntries (fastaEntry *firFasta, FILE *inFile, FILE *outFile) {
//Local variables
    char in;
    int count = 0, start = 0, end = 0;
    long i = 0;
    string title, sequence, scaffold, type;
    fastaEntry *curFasta = NULL;
//Loop the entire process per line of the GTF
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Read the scaffold
        initializeString (&scaffold);
        while (in != '\t') {
            readValueToString (&scaffold, in);
            in = fgetc (inFile);
        }
//Skip to entry type
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
//Read entry type
        initializeString (&type);
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&type, in);
            in = fgetc (inFile);
        }
//If the entry is a transcript, process it
        if (strcmp (type.str, "transcript") == 0) {
//Prep needed strings
            initializeString (&title);
            initializeString (&sequence);
//Find start and end values
            fscanf (inFile, "%d%d", &start, &end);
//skip to transcript_id
            while (in != ';') {
                in = fgetc (inFile);
            }
            while (in != '"') {
                in = fgetc (inFile);
            }
//Load transcript_id
            in = fgetc (inFile);
            while (in != '"') {
                readValueToString (&title, in);
                in = fgetc (inFile);
            }
//Find the corresponding scaffold
            curFasta = firFasta;
            while (strcmp (curFasta->title.str, scaffold.str) != 0) {
                curFasta = curFasta->next;
//To avoid infinite loops
                if (curFasta == NULL) {
                    printf ("%s not found in reference fasta!\n", scaffold.str);
                    exit (1);
                }
            }
//Copy the sequence substring
            for (i = (start - 1); i < (end - 1); i++) {
                readValueToString (&sequence, curFasta->sequence.str[i]);
            }
//Print the found data in fasta format
            fprintf (outFile, ">%s\n", title.str);
            for (i = 0; i < (sequence.len - 1); i++) {
                fprintf (outFile, "%c", sequence.str[i]);
                if ((i > 1) && ((i + 1) % 80 == 0)) {
                    fprintf (outFile, "\n");
                }
            }
            fprintf (outFile, "\n");
//Free strings
            free (sequence.str);
            sequence.len = 0;
            free (title.str);
            title.len = 0;
        }
//Skip to the end of the line
        while (in != '\n') {
            in = fgetc (inFile);
        }
//A counter so the user has some idea of how long it will take
        if (++count % 10000 == 0) {
            printf ("%d transcripts parsed...\n", count);
        }
//Reset transcript entry values
        free (scaffold.str);
        scaffold.len = 0;
        free (type.str);
        type.len = 0;
        start = 0;
        end = 0;
    }
    printf ("%d transcripts parsed.", count);
    return;
}
//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
