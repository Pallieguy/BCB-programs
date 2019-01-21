/* This program generates a pseudochromosomal fasta using BLASTN alignment of known markers to an established assembly as anchors, filling the resutling gaps with 'N'.  It takes an assembled genome fasta file, and a curated BLASTN output file as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Entry for the BLAST data
typedef struct blastEntry {
    string chr;
    int chrStart;
    int chrEnd;
    char chrStrand;
    string scaf;
    int scafStart;
    int scafEnd;
    char scafStrand;
    string evalue;
    struct blastEntry *next;
} blastEntry;

//Entry for fasta formatable data
typedef struct fastaEntry {
    string title;
    string seq;
    struct fastaEntry *next;
} fastaEntry;

//Entry for an assembled pseudochromosome
typedef struct pseuEntry {
    string pChr;
    string seq;
    int start;
    int end;
    int offset;
    struct pseuEntry *next;
} pseuEntry;

//List of functions, alphabetically
void assemblePseudochromosomes (fastaEntry *firFasta, FILE *inFile, FILE *pOutFile, FILE *tOutFile);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void freeFastaEntry (fastaEntry *oldEntry);
void initializeBlastEntry (blastEntry *newBlast);
void initializeFastaEntry (fastaEntry *newFasta);
void initializeString (string *newString);
void invertSequence (fastaEntry *curFasta);
void loadFastaList (fastaEntry *firFasta, FILE *inFile);
void readValueToString (string *string, char in);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Fasta_filename BLASTn_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *fInFile = NULL, *bInFile = NULL, *pOutFile = NULL, *tOutFile = NULL;
    fastaEntry firFasta;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&fInFile, argv[1], 'r');
    createFile (&bInFile, argv[2], 'r');
    createFile (&tOutFile, "Pseudochromosome_scaffold_locations.txt", 'w');
    createOutputFile (&pOutFile, argv[1]);
//Build dynamic linked list of scaf data
    printf ("Files found and created.  Compiling scaffold entries...\n");
    initializeFastaEntry (&firFasta);
    loadFastaList (&firFasta, fInFile);
    fclose (fInFile);
//Put together the scaffolds according to the alignment of the SNPs to the reference
    printf ("Assembling pseudochromosomes...\n");
    assemblePseudochromosomes (&firFasta, bInFile, pOutFile, tOutFile);
//Close everything
    printf ("Saved.  Closing files and freeing memory...\n");
    fclose (bInFile);
    fclose (pOutFile);
    fclose (tOutFile);
    printf ("Done.\n");
    return 0;
}

//Assembles Pseudochromosomes based on the BLASTN alignments, breaking whenever there is overlap
void assemblePseudochromosomes (fastaEntry *firFasta, FILE *inFile, FILE *pOutFile, FILE *tOutFile) {
//Local variables
    int pseuPos, count = 0, i, offset, usedCount = 0, start, end;
    blastEntry blast;
    pChrEntry firPChr, *curPchr;
    fastaEntry *curFasta = NULL, *prevFasta = NULL;
    initializePChrEntry (&firPChr);
    initializeBlastEntry (&blast);
//Automate the assembly process
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if ((feof (inFile)) || (ferror (inFile))) {
            break;
        }
//Get BLASTn values from the file (order is always the same);
        while (in != '\t') {
            readValueToString (blast->chr, in);
            in = fgetc (inFile);
        }
        fscanf (inFile, "%d%d", &blast->chrStart, &blast->chrEnd, 
        in = fgetc (inFile);
        blast->chrStrand = fgetc (inFile);
        in = fgetc (inFile);
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (blast->scaf, in);
            in = fgetc (inFile);
        }
        fscanf (inFile, "%d%d", &blast->scafStart, &blast->scafEnd);
        in = fgetc (inFile);
        blast->scafStrand = fgetc (inFile);
        in = fgetc (inFile);
        in = fgetc (inFile);
        while (in != '\n') {
            readValueToString (blast->evalue, in);
            in = fgetc (inFile);
        }
//Reset pointers to the start of the lists
        curFasta = firFasta;
        prevFasta = NULL;
//Find the scaffold from the list
        while (strcmp (curFasta->title.str, blast->scaf.str) != 0) {
            prevFasta = curFasta;
            curFasta = curFasta->next;
//If the scaffold isn't in the list anymore exit
            if (curFasta == NULL) {
                printf ("%s used twice.\n", blast->scaf.str);
                exit (3);
            }
        }
//If it's a new chromosome update the curChr and pseuPos
        if (strcmp (pChr.str, blast->chr.str) != 0) {
//The first BLASTn entry doesn't need to be freed
            if (pChr.len != 1) {
                free (pChr.str);
            }
            initializeString (&curChr);
            copyString (&curChr, &blast->chr);
            pseuPos = 1;
            curPChr->start = 1;
        }
//Check if the sequence needs to be inverted and find offsets
        if (blast->chrStrand != blast->scafStrand) {
            invertSequence (curFasta);
            if (blast->chrStrand == '-') {
                offset = (blast->chrEnd - blast->scafStart) - blast->scafStart;
            } else {
                offset = (blast->chrStart - blast->scafEnd) - blast->scafEnd;
            }
        } else {
            if (blast->chrStrand == '-') {
                offset = (blast->chrEnd - blast->scafEnd) - blast->scafEnd;
            } else {
                offset = (blast->chrStart - blast->scafStart) - blast->scafStart;
            }
        }
//For the first entry in a PChr
        if (pseuPos == 1) {
            copyString (curPChr->seq, curFasta->seq);
            curPChr->offset = offset;
            pseuPos = offset;
            pseuPos += curFasta->seq.len;
//If the start point is after the current position fill the Gap with `N`
        } else if (offset > pseuPos) {
            for (i = curPChr->end; i < offset; i++) {
                readValueToString (&curPChr->seq, 'N');
                pseuPos++;
            }
//Then copy the fasta sequence
            for (i = 0, i < curFasta->seq.len, i++) {
                readValueToString (curPChr->seq, curFasta->seq[i]);
                pseuPos++;
            }
//Otherwise start a new PChr
        } else {
            curPChr->end = pseuPos - curPChr->offset;
            curPChr->next = malloc (sizeof (*curPChr->next));
            initializePChrEntry (curPChr->next);
            curPChr = curPChr->next;
            copyString (curPChr->seq, curFasta->seq);
            pseuPos = offset + curFasta->seq.len + 1;
        }
//A counter so the user has some idea of how long it will take
        if (++count % 100 == 0){
            printf ("%d pseudochromosomes assembled...\n", count);
        }
//Clear the BLASTn data for the next round

    }
//Copy the sequence
        


//Update the titleData
        concatonateString (&curTitle->fasta.title, &curFasta->title);
        concatonateString (&curTitle->fasta.sequence, &curPseu->fasta.title);
        curTitle->end = (pseuPos - curPseu->start + 1);
        curTitle->start = curPseu->start;
        curTitle->length = curFasta->sequence.len - 1;
        curTitle->next = malloc (sizeof (*curTitle->next));
        initializePseuEntry (curTitle->next);
        prevTitle = curTitle;
        curTitle = curTitle->next;
//Add curFasta to curPseu and update curPseu->end and pseuPos
        concatonateString (&curPseu->fasta.sequence, &curFasta->sequence);
        pseuPos += (curFasta->sequence.len - 1);
        curPseu->end = pseuPos;
        usedCount++;
//Remove curFasta from the list
        if (curFasta != firFasta) {
            prevFasta->next = curFasta->next;
//Even if it's the first entry
        } else {
            firFasta = firFasta->next;
        }
//Remove curFasta from the list
        freeFastaEntry (curFasta);
//Remove firBlast from the list
        curBlast = firBlast;
        firBlast = firBlast->next;
        freeBlastEntry (curBlast);
    }
    printf ("%d pseudochromosomes assembled from %d scaffolds.\n", count, usedCount);
//Reset count to use it for the remaining fastaEntries
    count = 0;
    curFasta = firFasta;
//Count them
    while (curFasta != NULL) {
        count++;
        curFasta = curFasta->next;
    }
    printf ("%d scaffolds not used.  ", count);

    return;
}

//Copy a string's content to another string, replacing any content that may have been there before.
void copyString (string *destination, string *source) {
//Local variables
    int i;
    free (destination->str);
//Change size
    destination->len = source->len;
//realloc memory
    destination->str = malloc (destination->len);
//Copy values
    for (i = 0; i < destination->len; i++) {
        destination->str[i] = source->str[i];
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
    outTitle = malloc (j + 22);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_Pseudochromosomes.fa");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
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

//Inverts the order and nucleotide of a give sequence
void invertSequence (fastaEntry *curFasta) {
//Local variables
    int i = 0, j = (curFasta->seq.len - 1);
    string invSeq;
    initializeString (&invSeq);
//Allocate needed memory
    invSeq.len = curFasta->seq.len;
    invSeq.str = malloc (invSeq.len);
//Set the null terminator
    invSeq.str[j--] = '\0';
//Work backwards, inverting as we go
    while (j >= 0) {
        if (curFasta->seq.str[i] == 'A') {
            invSeq.str[j] = 'T';
        } else if (curFasta->seq.str[i] == 'C') {
            invSeq.str[j] = 'G';
        } else if (curFasta->seq.str[i] == 'G') {
            invSeq.str[j] = 'C';
        } else if (curFasta->seq.str[i] == 'T') {
            invSeq.str[j] = 'A';
        } else {
            invSeq.str[j] = 'N';
        }
        j--;
        i++;
    }
//Replace the old sequence with the new one
    copyString (&curFasta->seq, &invSeq);
    return;
}

//Loads the entries from a fasta file into a linked list
void loadFastaList (fastaEntry *curFasta, FILE *inFile) {
//Local variables
    char in;
    int count = 1;
//Skip any headers
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
//Load the first title up to a blank space
    in = fgetc (inFile);
    while ((in != '\n') && (in != '\t') && (in != ' ')) {
        readValueToString (&curFasta->title, in);
        in = fgetc (inFile);
    }
//If there is more than jsut the title, ignore it
    while (in != '\n') {
        in = fgetc (inFile);
    }
//Automate the rest
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
//If it's a new entry, create a new node and load the title
        }
        if (in == '>') {
            curFasta->next = malloc (sizeof (*curFasta->next));
            initializeFastaEntry (curFasta->next);
            curFasta = curFasta->next;
            in = fgetc (inFile);
//Read the new title
            while ((in != '\n') && (in != '\t') && (in != ' ')) {
                readValueToString (&curFasta->title, in);
                in = fgetc (inFile);
            }
//If there is more than jsut the title, ignore it
            while (in != '\n') {
                in = fgetc (inFile);
            }
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0){
                printf ("%d fasta entries loaded...\n", count);
            }
        } else if (in != '\n') {
            readValueToString (&curFasta->sequence, in);
        }
    }
    printf ("%d entries loaded.  ", count);
    return;
}

//Save fasta to file
void printFastaEntry (fastaEntry *fasta, FILE *outFile) {
//Local variables
    int location = 0;
//Print the title
    fprintf (outFile, ">%s\n", fasta->title.str);
//Print the sequence 
    while (location < (fasta->sequence.len - 1)) {
        if (fasta->sequence.str[location] == 'A') {
            fprintf (outFile, "A");
        } else if (fasta->sequence.str[location] == 'T') {
            fprintf (outFile, "T");
        } else if (fasta->sequence.str[location] == 'G') {
            fprintf (outFile, "G");
        } else if (fasta->sequence.str[location] == 'C') {
            fprintf (outFile, "C");
        } else {
            fprintf (outFile, "N");
        }
        location++;
//Add a carriage return every 80 characters
        if (location % 80 == 0) {
            fprintf (outFile, "\n");
        }
    }
    fprintf (outFile, "\n");
    return;
}

//Save the Pseudochromosomes in a fasta format
void printPseudochromosomes (pseuEntry *firPseu, fastaEntry *firFasta, pseuEntry *firTitle, FILE *pOutFile, FILE *tOutFile) {
//Local variables
    fastaEntry *curFasta = firFasta;
    pseuEntry *curPseu = firPseu, *curTitle = firTitle, *firChrPseu = firPseu;
    int lowest = 0;
    string curChr, tempTitle;
//Load the first chr
    initializeString (&curChr);
    concatonateString (&curChr, &curPseu->fasta.title);
//Go until the list is done
    while (curPseu != NULL) {
//Find the lowest start value for a chromosome
        while ((curPseu != NULL) && (strcmp (curPseu->fasta.title.str, curChr.str) == 0)) {
            if (curPseu->start < lowest) {
                lowest = curPseu->start;
            }
            curPseu = curPseu->next;
        }
//Go back to the start of the list
        curPseu = firChrPseu;
//Adjust the lowest value to become the difference to add
        lowest = (lowest * -1) + 1;
//Update curPseu and associated Titles
        while ((curPseu!= NULL) && (strcmp (curPseu->fasta.title.str, curChr.str) == 0)) {
//Generate the proper pseudochromosome title
            free (tempTitle.str);
            initializeString (&tempTitle);
            sprintf (tempTitle.str, "%s_%d-%d", curPseu->fasta.title.str, (curPseu->start + lowest), (curPseu->end + lowest));
            tempTitle.len = (strlen (tempTitle.str) + 1);
//Update curPseu->fasta.title
            copyString (&curPseu->fasta.title, &tempTitle);
//Adjust the scaffold titles to match the pseudochromosomes
            while ((curTitle != NULL) && (curTitle->start == curPseu->start)) {
                copyString (&curTitle->fasta.sequence, &tempTitle);
                curTitle = curTitle->next;
            }
//Move to the next pseuEntry
            curPseu = curPseu->next;
        }
//Update the curChr;
        if (curPseu != NULL) {
            copyString (&curChr, &curPseu->fasta.title);
            firChrPseu = curPseu;
        }
    }
//Print the pseudochromosomes
    curPseu = firPseu;
    while (curPseu != NULL) {
        printFastaEntry (&curPseu->fasta, pOutFile);
        curPseu = curPseu->next;
        freePseuEntry (firPseu);
        firPseu = curPseu;
    }
//Print the scaffolds
    while (curFasta != NULL) {
        printFastaEntry (curFasta, pOutFile);
        curFasta = curFasta->next;
        freeFastaEntry (firFasta);
        firFasta = curFasta;
    }
//Print the scaffold shift data
    printTitles (firTitle, tOutFile);
    return;
}

//Save the transition data for the titles used
void printTitles (pseuEntry *firTitle, FILE *tOutFile) {
//Local variables
    pseuEntry *curTitle = firTitle;
//Print them all
    while (curTitle != NULL) {
        fprintf (tOutFile, "%s\t%s:%d-%d\t%c\n", curTitle->fasta.title.str, curTitle->fasta.sequence.str, curTitle->end, curTitle->length, curTitle->inverted);
        firTitle = firTitle->next;
        freePseuEntry (curTitle);
        curTitle = firTitle;
    }
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[((string->len) - 2)] = in;
    string->str[((string->len) - 1)] = '\0';
    return;
}
