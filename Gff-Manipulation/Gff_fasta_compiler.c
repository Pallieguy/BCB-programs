/* This program turns the gene or transcript entries in a gff3 into a fasta.  It takes a gff3 and a fasta file as input. */

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
    char strand;
    string notes;
    struct gff *next;
} gff;

//List of functions, alphabetically
void copyString (string *dest, string *src);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
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
    createOutputFile (&outFile, argV[1]);
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

//Free a gff DLL
void freeGff (gff *entry) {
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

//Sets minimum values to a gff entry
void initializeGff (gff *newGff) {
    initializeString (&newGff->scaffold);
    initializeString (&newGff->type);
    initializeString (&newGff->notes);
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
//In case it's the end of the file
                if (feof) {
                    break;
                }
            }
        } else {
//Read the scaffold
            while (in != '\t') {
                readValueToString (&curGff->scaffold, in);
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
                readValueToString (&curGff->type, in);
                in = fgetc (inFile);
            }
//Only copy gene or transcript entries
            if ((strcmp (curGff->type.str, "gene") == 0) || (strcmp (curGff->type.str, "transcript") == 0)) { //Removed to cover all entries */
//Read the start, end, and strand
                fscanf (inFile, "%d%d%*c%*c%*c%c%*c%*c%*c", &curGff->start, &curGff->end, &curGff->strand);
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
                                            readValueToString (&curGff->notes, in);
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
        if (++count % 50000 == 0) {
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
                printf ("%s not found in fasta file!\n", curGff->scaffold.str);
                exit (3);
            }
        }
//Start a loop for clustered genes
        while (1) {
//Print fasta title
            fprintf (outFile, ">%s_%s_%d-%d_%c\t%s\n", curGff->scaffold.str, curGff->type.str, curGff->start, curGff->end, curGff->strand, curGff->notes.str);
//Copy sequence
            initializeString (&sequence);
            for (i = curGff->start; i < curGff->end; i++) {
                readValueToString (&sequence, curFasta->sequence.str[i]);
            }
//Invert if needed
            if (curGff->strand == '-') {
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
            prevGff = curGff;
            curGff = curGff->next;
//If it's the end of the cluster or the gff list
            if ((curGff == NULL) || (strcmp (prevGff->scaffold.str, curGff->scaffold.str) != 0)) {
                freeGff (prevGff);
                break;
            } else {
                freeGff (prevGff);
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
