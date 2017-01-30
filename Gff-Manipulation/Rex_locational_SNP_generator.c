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

//SNP entry
typedef struct SNP {
    string location;
    int start;
    int end;
    char strand;
    string title;
} SNP;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void findSNPs (fasta *firFasta, FILE *inFile, FILE *outFile);
void freeFasta (fasta *entry);
void freeFastaList (fasta *firFasta);
void initializeFasta (fasta *newFasta);
void initializeSNP (SNP *newSNP);
void initializeString (string *newString);
void loadFastaList (fasta *firFasta, FILE *inFile);
void loadSNPList (SNP *firSNP, FILE *inFile);
void printfSNPEntries (fasta *entry, FILE *outFile);
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
    findSNPs (firFasta, bInFile outFile);
//Close everything and free memory
    printf ("  Closing files and freeing memory...\n");
    fclose (binFile);
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

//Free a SNP DLL
void freeSNP (SNP *entry) {
    free (entry->scaffold.str);
    free (entry->type.str);
    free (entry->notes.str);
    free (entry);
    return;
}

//Sets minimum values to a fasta entry
void initializeFasta (fasta *newFasta) {
    initializeString (&newFasta->title);
    initializeString (&newFasta->sequence);
    newFasta->next = NULL;
    return;
}

//Sets minimum values to a SNP entry
void initializeSNP (SNP *newSNP) {
    initializeString (&newSNP->scaffold);
    initializeString (&newSNP->type);
    initializeString (&newSNP->notes);
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

//Read SNP data from file into a DLL
void loadSNPList (SNP *firSNP, FILE *inFile) {
//Local variables
    int count = 0;
    char in;
    SNP *curSNP = firSNP, *prevSNP = NULL;
//Skip the SNP header line
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
//Loop for the whole file
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//# is a comment line and can be skipped
        if (in == '#') {
            while (in != '\n') {
                in = fgetc (inFile);
            }
            count--;
        } else {
//Read the scaffold
            while (in != '\t') {
                readValueToString (&curSNP->scaffold, in);
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
//Skip to type
            while (in != '\t') {
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
//Read the type
            while (in != '\t') {
                readValueToString (&curSNP->type, in);
                in = fgetc (inFile);
            }
//Only copy gene or transcript entries
            if ((strcmp (curSNP->type.str, "gene") == 0) || (strcmp (curSNP->type.str, "transcript") == 0)) { //Removed to cover all entries */
//Read the start, end, and strand
                fscanf (inFile, "%d%d%*c%*c%*c%c%*c%*c%*c", &curSNP->start, &curSNP->end, &curSNP->strand);
                in = fgetc (inFile);
//Scan the comments for "Notes="
                while (in != '\n') {
                    in = fgetc (inFile);
//Check a letter at a time
                    if (in == 'N') {
                        in = fgetc (inFile);
                        if (in == 'o') {
                            in = fgetc (inFile);
                            if (in =='t') {
                                in = fgetc (inFile);
                                if (in == 'e') {
                                    in = fgetc (inFile);
                                    if (in == '=') {
                                        in = fgetc (inFile);
                                        while (in != ';') {
                                            readValueToString (&curSNP->notes, in);
                                            in = fgetc (inFile);
                                        }
                                    }
                                }
                            }
                        }
                    }
//Skip to the next comment if it's not a note
                    while ((in != ';') && (in != '\n')) {
                        in = fgetc (inFile);
                    }
                }
//Create a new entry on the SNP DLL
                curSNP->next = malloc (sizeof (*curSNP));
                initializeSNP (curSNP->next);
                prevSNP = curSNP;
                curSNP = curSNP->next;
//If it's not a gene, skip it and empty the scaffold entry
            } else {
                while (in != '\n') {
                    in = fgetc (inFile);
                }
                initializeString (&curSNP->scaffold);
                initializeString (&curSNP->type);
            }
//In either case empty type
        }
//A counter so the user has some idea of how long it will take
        if ((++count % 50000 == 0) && (count != 0)) {
            printf ("%d entries loaded...\n", count);
        }
    }
    prevSNP->next = NULL;
    freeSNP (curSNP);
    printf ("%d entries loaded.", count);
    return;
}

//Prints the fasta sequences identified by SNP coordinates
void printfSNPEntries (SNP *curSNP, fasta *firFasta, FILE *outFile) {
//Local variables
    int count = 0, i;
    SNP *prevSNP = NULL;
    fasta *curFasta = NULL;
    string sequence;
    initializeString (&sequence);
//Loop to cover SNP list
    while (curSNP != NULL) {
//Start at the first fasta entry
        curFasta = firFasta;
//Find matching scaffold
        while (strcmp (curSNP->scaffold.str, curFasta->title.str) != 0) {
            curFasta = curFasta->next;
//In case an entry is missing
            if (curFasta == NULL) {
                printf ("\"%s\" not found in fasta file!\n", curSNP->scaffold.str);
                exit (3);
            }
        }
//Start a loop for clustered genes
        while (1) {
//Print fasta title
            fprintf (outFile, ">%s_%s_%d-%d_%c\t%s\n", curSNP->scaffold.str, curSNP->type.str, curSNP->start, curSNP->end, curSNP->strand, curSNP->notes.str);
//Copy sequence
            initializeString (&sequence);
            for (i = curSNP->start; i < curSNP->end; i++) {
                readValueToString (&sequence, curFasta->sequence.str[i]);
            }
//Invert if needed
            if (curSNP->strand == '-') {
                invertSequence (&sequence);
            }
//Print the sequence
            for (i = 1; i < sequence.len; i++) {
                fprintf (outFile, "%c", sequence.str[(i - 1)]);
                if (i % 80 == 0) {
                    fprintf (outFile, "\n");
                }
            }
            fprintf (outFile, "\n");
            free (sequence.str);
//Move to next entry, free last one
            prevSNP = curSNP;
            curSNP = curSNP->next;
//If it's the end of the cluster or the SNP list
            if ((curSNP == NULL) || (strcmp (prevSNP->scaffold.str, curSNP->scaffold.str) != 0)) {
                freeSNP (prevSNP);
                break;
            } else {
                freeSNP (prevSNP);
            }            
        }
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0) {
            printf ("%d entries written...\n", count);
        }
    }
    printf ("%d entries written.", count);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
