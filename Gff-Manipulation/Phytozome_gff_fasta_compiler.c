/* This program turns the entries in a gff3 into a fasta.  It takes a gff3 from phytozome and a fasta file as input. */

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

//Gff entry
typedef struct gff {
    string scaffold;
    string type;
    int start;
    int end;
    string id;
    struct gff *next;
} gff;

//List of functions, alphabetically
void copyString (string *dest, string *src);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix);
void freeFasta (fasta *entry);
void freeFastaList (fasta *firFasta);
void freeGff (gff *firGff);
void initializeFasta (fasta *newFasta);
void initializeGff (gff *newGff);
void initializeString (string *newString);
void invertSequence (string *sequence);
void loadFastaList (fasta *firFasta, FILE *inFile);
void loadGffList (gff *firGff, FILE *inFile);
void printfGffEntries (gff *curGff, fasta *firFasta, FILE *outFile);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 3) {
        printf ("Usage: %s Gff3_filename Fasta_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *gInFile = NULL, *fInFile = NULL, *outFile = NULL;
    gff *firGff = NULL;
    fasta *firFasta = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&gInFile, argV[1], 'r');
    createFile (&fInFile, argV[2], 'r');
    createOutputFile (&outFile, argV[1], NULL, ".fa");
//Load gff entries
    printf ("Files opened.  Loading gff entries...\n");
    firGff = malloc (sizeof (*firGff));
    initializeGff (firGff);
    loadGffList (firGff, gInFile);
    fclose (gInFile);
//Load fasta entries
    printf ("  Loading fasta entries...\n");
    firFasta = malloc (sizeof (*firFasta));
    initializeFasta (firFasta);
    loadFastaList (firFasta, fInFile);
    fclose (fInFile);
//Print gff referenced sequences
    printf ("  Writing gff sequences...\n");
    printfGffEntries (firGff, firFasta, outFile);
//Close everything and free memory
    printf ("  Closing files and freeing memory...\n");
    freeFastaList (firFasta);
    fclose (outFile);
    return 0;
}

//Copy a string's content to another string, replacing any content that may have been there before.
void copyString (string *dest, string *src) {
//Local variables
    int i;
    free (dest->str);
//Change size
    dest->len = src->len;
//realloc memory
    dest->str = malloc (dest->len);
//Copy values
    for (i = 0; i < dest->len; i++) {
        dest->str[i] = src->str[i];
    }
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

//Free a gff DLL
void freeGff (gff *entry) {
    free (entry->scaffold.str);
    free (entry->type.str);
    free (entry->id.str);
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

//Sets minimum values to a gff entry
void initializeGff (gff *newGff) {
    initializeString (&newGff->scaffold);
    initializeString (&newGff->type);
    initializeString (&newGff->id);
    newGff->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Inverts the order and nucleotide of a give sequence
void invertSequence (string *sequence) {
//Local variables
    int i = 0, j = (sequence->len - 1);
    string invSequence;
    initializeString (&invSequence);
//Allocate needed memory
    invSequence.len = sequence->len;
    invSequence.str = malloc (invSequence.len);
//Set the null terminator
    invSequence.str[j] = '\0';
    j--;
//Work backwards, inverting as we go
    while (j >= 0) {
        if (sequence->str[i] == 'A') {
            invSequence.str[j] = 'T';
        } else if (sequence->str[i] == 'C') {
            invSequence.str[j] = 'G';
        } else if (sequence->str[i] == 'G') {
            invSequence.str[j] = 'C';
        } else if (sequence->str[i] == 'N') {
            invSequence.str[j] = 'N';
        } else if (sequence->str[i] == 'T') {
            invSequence.str[j] = 'A';
        }
        j--;
        i++;
    }
//Replace the old sequence with the new one
    copyString (sequence, &invSequence);
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

//Read gff data from file into a DLL
void loadGffList (gff *firGff, FILE *inFile) {
//Local variables
    int count = 0;
    char in;
    gff *curGff = firGff, *prevGff = NULL;
//Skip the gff header line
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
                readValueToString (&curGff->scaffold, in);
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
//Skip to start
            while (in != '\t') {
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
//Read the type
            while (in != '\t') {
                readValueToString (&curGff->type, in);
                in = fgetc (inFile);
            }
//Only copy gene or transcript entries
            if ((strcmp (curGff->type.str, "gene") == 0) || (strcmp (curGff->type.str, "transcript") == 0)) { //Removed to cover all entries */
//Read the start and end
                fscanf (inFile, "%d%d", &curGff->start, &curGff->end);
                in = fgetc (inFile);
//Scan the comments for "ID="
                while (in != '\n') {
                    in = fgetc (inFile);
//Check a letter at a time
                    if (in == 'I') {
                        in = fgetc (inFile);
                        if (in == 'D') {
                            in = fgetc (inFile);
                            if (in == '=') {
                                in = fgetc (inFile);
                                while (in != ';') {
                                    readValueToString (&curGff->id, in);
                                    in = fgetc (inFile);
                                }
                            }
                        }
                    }
                }
//Create a new entry on the gff DLL
                curGff->next = malloc (sizeof (*curGff));
                initializeGff (curGff->next);
                prevGff = curGff;
                curGff = curGff->next;
//If it's not a gene, skip it and empty the scaffold entry
            } else {
                while (in != '\n') {
                    in = fgetc (inFile);
                }
                initializeString (&curGff->scaffold);
                initializeString (&curGff->type);
            }
//In either case empty type
        }
//A counter so the user has some idea of how long it will take
        if ((++count % 50000 == 0) && (count != 0)) {
            printf ("%d entries loaded...\n", count);
        }
    }
    prevGff->next = NULL;
    freeGff (curGff);
    printf ("%d entries loaded.", count);
    return;
}

//Prints the fasta sequences identified by gff coordinates
void printfGffEntries (gff *curGff, fasta *firFasta, FILE *outFile) {
//Local variables
    int count = 0, i;
    gff *prevGff = NULL;
    fasta *curFasta = NULL;
    string sequence;
    initializeString (&sequence);
//Loop to cover gff list
    while (curGff != NULL) {
//Start at the first fasta entry
        curFasta = firFasta;
//Find matching scaffold
        while (strcmp (curGff->scaffold.str, curFasta->title.str) != 0) {
            curFasta = curFasta->next;
//In case an entry is missing
            if (curFasta == NULL) {
                printf ("\"%s\" not found in fasta file!\n", curGff->scaffold.str);
                exit (3);
            }
        }
//Start a loop for clustered genes
        while (1) {
//Print fasta title
            fprintf (outFile, ">%s\n", curGff->id.str);
//Copy sequence
            initializeString (&sequence);
            for (i = curGff->start; i < curGff->end; i++) {
                readValueToString (&sequence, curFasta->sequence.str[i]);
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
            prevGff = curGff;
            curGff = curGff->next;
//A counter so the user has some idea of how long it will take
            if (++count % 10000 == 0) {
                printf ("%d entries written...\n", count);
            }         
//If it's the end of the cluster or the gff list
            if ((curGff == NULL) || (strcmp (prevGff->scaffold.str, curGff->scaffold.str) != 0)) {
                freeGff (prevGff);
                break;
            } else {
                freeGff (prevGff);
            }
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
