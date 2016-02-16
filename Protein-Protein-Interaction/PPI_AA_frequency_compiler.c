/* This program calculates the amino acid frequency of a list of proteins and generates a csv of the paired counts of known interacting and non-interacting pairs.  It takes a fasta file, a human interacting file, a yeast interacting file, and a non-interacting file as input. */

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
    string seq;
    string output;
    struct fastaEntry *next;
} fastaEntry;

//List of functions, alphabetically
void calculateAAFreq (fastaEntry *entry);
void createFastaEntry (fastaEntry **newEntry);
void createFastaList (fastaEntry *entry, FILE *input);
void createFile (FILE **file, char *fName, char perm);
void crossCheck (fastaEntry *firFasta, char source, char matchType, FILE *inFile, FILE *outFile);
void freeFastaEntry (fastaEntry *entry);
void freeFastaList (fastaEntry *entry);
void initializeString (string *newString);
void printCSVEntry (fastaEntry *firMatch, fastaEntry *secMatch, char source, char matchType, FILE *outFile);
void readValueToString (string *string, char in);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 6) {
        printf ("Usage: %s Fasta_filename Human_interactions Yeast_interactions Negative_interactions Output_CSV_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    fastaEntry *firFasta = NULL;
    FILE *inFile, *fasFile, *outFile;
//File creation and checks
    printf ("Opening initial files...\n");
    createFile (&fasFile, argv[1], 'r');
    createFile (&inFile, argv[2], 'r');
    createFile (&outFile, argv[5], 'w');
    fprintf (outFile, "Source,Protein,A,C,D,E,F,G,H,I,K,L,M,N,P,Q,R,S,T,V,W,Y,Class\n");
//Build dynamic linked list of scaf data
    printf ("Files found and created.  Compiling fasta entries...\n");
    createFastaEntry (&firFasta);
    createFastaList (firFasta, fasFile);
    fclose (fasFile);
//Find the entries from the Human interaction file
    printf ("Compiled.  Cross referencing with Human interaction profile...\n");
    crossCheck (firFasta, 'H', 'I', inFile, outFile);
    fclose (inFile);
//Find the entries from the Yeast interaction file
    printf ("Cross referenced.  Cross referencing with Yeast interaction profile...\n");
    createFile (&inFile, argv[3], 'r');
    crossCheck (firFasta, 'Y', 'I', inFile, outFile);
    fclose (inFile);
//Find the entries from the Negative interaction file
    printf ("Cross referenced.  Cross referencing with Negative interaction profile...\n");
    createFile (&inFile, argv[4], 'r');
    crossCheck (firFasta, 'N', 'N', inFile, outFile);
    fclose (inFile);
//Close everything
    printf ("Cross referenced.  Freeing memory and closing files...\n");
    fclose (outFile);
    freeFastaList (firFasta);
    printf ("Done.\n");
    return 0;
}

//Determine the output to be printed for each fasta entry
void calculateAAFreq (fastaEntry *entry) {
//Local variables
    int A = 0, C = 0, D = 0, E = 0, F = 0, G = 0, H = 0, I = 0, K = 0, L = 0, M = 0, N = 0, P = 0, Q = 0, R = 0, S = 0, T = 0, V = 0, W = 0, Y = 0, pos = 0;
//Go through every position
    while (pos < entry->seq.len) {
//20 amino acids = 20 posibilities
        switch (entry->seq.str[pos]) {
            case 'A':
                A++;
                break;
            case 'C':
                C++;
                break;
            case 'D':
                D++;
                break;
            case 'E':
                E++;
                break;
            case 'F':
                F++;
                break;
            case 'G':
                G++;
                break;
            case 'H':
                H++;
                break;
            case 'I':
                I++;
                break;
            case 'K':
                K++;
                break;
            case 'L':
                L++;
                break;
            case 'M':
                M++;
                break;
            case 'N':
                N++;
                break;
            case 'P':
                P++;
                break;
            case 'Q':
                Q++;
                break;
            case 'R':
                R++;
                break;
            case 'S':
                S++;
                break;
            case 'T':
                T++;
                break;
            case 'V':
                V++;
                break;
            case 'W':
                W++;
                break;
            case 'Y':
                Y++;
                break;
        }
        pos++;
    }

//Realloc to uniform size
    entry->output.len = 100;
    entry->output.str = realloc (entry->output.str, 100);
//Load values
    sprintf (entry->output.str, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d", A, C, D, E, F, G, H, I, K, L, M, N, P, Q, R, S, T, V, W, Y);
    return;
}

//Create a new node on a fasta entry list
void createFastaEntry (fastaEntry **newEntry) {
    *newEntry = malloc (sizeof (**newEntry));
    initializeString (&(*newEntry)->title);
    initializeString (&(*newEntry)->seq);
    initializeString (&(*newEntry)->output);
    (*newEntry)->next = NULL;
    return;
}

//Generates a linked list holding the scaffold data
void createFastaList (fastaEntry *entry, FILE *input) {
//Local variables
    char in;
    while (1) {
        in = fgetc (input);
//Break conditions
        if (((ferror (input)) || (feof(input)))) {
            break;
//Check if it's a title
        } else if (in == '>') {
            in = fgetc (input);
//Create a new node at the new title
            if (entry->seq.len != 0) {
                calculateAAFreq (entry);
                createFastaEntry(&entry->next);
                entry = entry->next;
            }
//Read line to curScaf->title
            while (in != '\n') {
                readValueToString (&entry->title, in);
                in = fgetc (input);
            }
//Read line to sequence extending as needed
        } else if (in != '\n') {
            readValueToString (&entry->seq, in);
        }
    }
//This is the last fasta entry being read, the loop will be finished so it needs to explictly be dealt with
    calculateAAFreq (entry);
    return;
}

//Check if the file was created properly
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

//Find the entries in the interaction file and create a CSV entry for each pair
void crossCheck (fastaEntry *firFasta, char source, char matchType, FILE *inFile, FILE *outFile) {
//Local variables
    fastaEntry *firMatch = NULL, *secMatch = NULL, *curFasta = NULL,*miss1 = NULL, *miss2 = NULL;;
    char title1[7], title2[7], line[1000];
//Initialize the empty entries
    createFastaEntry (&miss1);
    calculateAAFreq (miss1);
    createFastaEntry (&miss2);
    calculateAAFreq (miss2);
//Read the interaction file for the titles
while (fscanf (inFile, "%[^\n]%*c", line) != EOF) {
//make sure the line is Data not header
        if (line[0] == '#') {
            fscanf (inFile, "%[^\n]%*c", line);
        }
//Parse titles as needed
        if (source == 'N') {
            sscanf (line, "%s%s", title1, title2);
        } else {
            sscanf (line, "%*s%s%s", title1, title2);
        }
//Search through the fasta list for matches
        curFasta = firFasta;
        while (((firMatch == NULL) || (secMatch == NULL)) && (curFasta != NULL)) {
//Only use the positive fasta entries
            if (strcmp (title1, curFasta->title.str) == 0) {
                firMatch = curFasta;
            }
            if (strcmp (title2, curFasta->title.str) == 0) {
                secMatch = curFasta;
            }
            curFasta = curFasta->next;
        }
//In case a title isn't in the fasta list
        if (curFasta == NULL) {
//Check firMatch
            if (firMatch == NULL) {
//Update the title of the empty entry to the missing title
                int i;
                for (i = 0; i < 6; i++) {
                    readValueToString (&miss1->title, title1[i]);
                }
//Point the missing match at the empty entry
                firMatch = miss1;
            }
//Check secMatch
            if (secMatch == NULL) {
//Update the title of the empty entry to the missing title
                int i;
                for (i = 0; i < 6; i++) {
                    readValueToString (&miss2->title, title2[i]);
                }
//Point the missing match at the empty entry
                secMatch = miss2;
            }
        }
//Pass the matches to be printed
        printCSVEntry (firMatch, secMatch, source, matchType, outFile);
        firMatch = NULL;
        secMatch = NULL;
    }
//Free the empty entries
    freeFastaEntry (miss1);
    freeFastaEntry (miss2);
    return;
}

//Frees all data alloc'd by createFastaEntry()
void freeFastaEntry (fastaEntry *entry) {
    free (entry->title.str);
    free (entry->seq.str);
    free (entry->output.str);
    free (entry);
    return;
}

//Free the entire DLL of fastaEntry's
void freeFastaList (fastaEntry *entry) {
    fastaEntry *prevEntry;
    while (entry != NULL) {
        prevEntry = entry;
        entry = entry->next;
        freeFastaEntry (prevEntry);
    }
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (sizeof (*newString->str));
    newString->str[0] = '\0';
    return;
}

//Print the appropriate data to a CSV
void printCSVEntry (fastaEntry *firMatch, fastaEntry *secMatch, char source, char matchType, FILE *outFile) {
    fprintf (outFile, "%c,%c1_%s,%s,%c\n%c,%c2_%s,%s,%c\n", source, matchType, firMatch->title.str, firMatch->output.str, matchType, source, matchType, secMatch->title.str, secMatch->output.str, matchType);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[((string->len) - 2)] = in;
    string->str[((string->len) - 1)] = '\0';
    return;
}
