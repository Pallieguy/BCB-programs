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
    string score;
    struct blastEntry *next;
} blastEntry;

//Entry for fasta formatable data
typedef struct fastaEntry {
    string title;
    string sequence;
    struct fastaEntry *next;
} fastaEntry;

//Entry for an assembled pseudochromosome
typedef struct pseuEntry {
    fastaEntry fasta;
    int start;
    int end;
    int length;
    char inverted;
    struct pseuEntry *next;
} pseuEntry;

//List of functions, alphabetically
fastaEntry assemblePseudochromosomes (fastaEntry *firFasta, blastEntry *firBlast, pseuEntry *firPseu, pseuEntry *firTitle);
void concatonateString (string *destination, string *source);
void copyString (string *destination, string *source);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void findBestBlast (blastEntry *curBlast, blastEntry *bestMatch);
void freeBlastEntry (blastEntry *oldEntry);
void freeFastaEntry (fastaEntry *oldEntry);
void freePseuEntry (pseuEntry *oldEntry);
void initializeBlastEntry (blastEntry *newBlast);
void initializeFastaEntry (fastaEntry *newFasta);
void initializePseuEntry (pseuEntry *newPseu);
void initializeString (string *newString);
void invertSequence (fastaEntry *curFasta);
void loadBlastList (blastEntry *curBlast, FILE *inFile);
void loadFastaList (fastaEntry *firFasta, FILE *inFile);
void printFastaEntry (fastaEntry *fasta, FILE *outFile);
void printPseudochromosomes (pseuEntry *firPseu, fastaEntry *firFasta, pseuEntry *firTitle, FILE *pOutFile, FILE *tOutFile);
void printTitles (pseuEntry *firTitle, FILE *tOutFile);
void readValueToString (string *string, char in);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Genome_fasta_filename BLASTN_results_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *gInFile = NULL, *bInFile = NULL, *pOutFile = NULL, *tOutFile = NULL;
    fastaEntry firFasta;
    blastEntry firBlast;
    pseuEntry firPseu, firTitle;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&gInFile, argv[1], 'r');
    createFile (&bInFile, argv[2], 'r');
    createFile (&tOutFile, "Pseudochromosome_scaffold_locations.txt", 'w');
    createOutputFile (&pOutFile, argv[1]);
//Build dynamic linked list of scaf data
    printf ("Files found and created.  Compiling scaffold entries...\n");
    initializeFastaEntry (&firFasta);
    loadFastaList (&firFasta, gInFile);
    fclose (gInFile);
//Build a dynamic linked list of the BLASTN data
    printf ("Compiling BLASTN entries...\n");
    initializeBlastEntry (&firBlast);
    loadBlastList (&firBlast, bInFile);
    fclose (bInFile);
//Put together the scaffolds according to the alignment of the SNPs to the reference
    printf ("Assembling pseudochromosomes...\n");
    initializePseuEntry (&firPseu);
    initializePseuEntry (&firTitle);
    firFasta = assemblePseudochromosomes (&firFasta, &firBlast, &firPseu, &firTitle);
//Print the pseudochromosomes in a structured fashion, free after writing
    printf ("Saving data...\n");
    printPseudochromosomes (&firPseu, &firFasta, &firTitle, pOutFile, tOutFile);
//Close everything
    printf ("Saved.  Closing files and freeing memory...\n");
    fclose (pOutFile);
    fclose (tOutFile);
    printf ("Done.\n");
    return 0;
}

//Assembles Pseudochromosomes based on the BLASTN alignments, breaking whenever there is overlap
fastaEntry assemblePseudochromosomes (fastaEntry *firFasta, blastEntry *firBlast, pseuEntry *firPseu, pseuEntry *firTitle) {
//Local variables
    int pseuPos = 1, count = 0, i, offset = 0, usedCount = 0;
    blastEntry *curBlast = NULL, *bestMatch = NULL;
    fastaEntry *curFasta = NULL, *prevFasta = NULL;
    pseuEntry *curPseu = firPseu, *curTitle = firTitle, *prevTitle = NULL;
    string curChr;
    initializeString (&curChr);
//Will go until all BLASTN entries are used
    while (firBlast != NULL) {
//Reset pointers to the start of the lists
        curFasta = firFasta;
        prevFasta = NULL;
        curBlast = firBlast;
//Find the scaffold from the list
        while (strcmp (curFasta->title.str, curBlast->scaf.str) != 0) {
            prevFasta = curFasta;
            curFasta = curFasta->next;
//If the scaffold isn't in the list anymore exit
            if (curFasta == NULL) {
                printf ("%s used twice.\n", curBlast->scaf.str);
                exit (3);
            }
        }
//If there are multiple matches to a scaffold, find the best evalue
        while (strcmp (curBlast->scaf.str, curBlast->next->scaf.str) == 0) {
            findBestBlast (curBlast, bestMatch);
//Remove the lower match from the BLASTN list, since this only compares 2 matches at at time it will alwys be firBlast or firBlast->next
            if (bestMatch == firBlast) {
                curBlast = firBlast->next;
                firBlast->next = curBlast->next;
            } else {
                firBlast = firBlast->next;
            }
            freeBlastEntry (curBlast);
            curBlast = firBlast;
//At the end of the Blast list the process needs to be stopped
            if (curBlast->next == NULL) {
                break;
            }
        }
//The best match will always end up being the first match in the list, declare it in case there is only one match to that scaffodl
        bestMatch = firBlast;
//Find the start location of the scaffold based on bestMatch
        offset = (bestMatch->chrStart - bestMatch->scafStart) - bestMatch->scafStart;
//If it's a new chromosome update the curChr and pseuPos
        if (strcmp (curChr.str, bestMatch->chr.str) != 0) {
//Free only if it's not the first one
            if (curChr.len != 1) {
                free (curChr.str);
                initializeString (&curChr);
                curPseu->next = malloc (sizeof (*curPseu->next));
                initializePseuEntry (curPseu->next);
                curPseu = curPseu->next;
            }
            concatonateString (&curChr, &bestMatch->chr);
            concatonateString (&curPseu->fasta.title, &curChr);
            pseuPos = 1;
            curPseu->start = offset;
        }
//If the start point is after the current position fill the Gap with `N`
        if ((offset > pseuPos) && (pseuPos != 1)) {
            for (i = curPseu->end; i < offset; i++) {
                readValueToString (&curPseu->fasta.sequence, 'N');
                pseuPos++;
            }
//Otherwise make a new curPseu if needed and update the curPseu start
        } else {
             if (pseuPos != 1) {
                curPseu->next = malloc (sizeof (*curPseu->next));
                initializePseuEntry (curPseu->next);
                curPseu = curPseu->next;
                curPseu->start = offset;
                concatonateString (&curPseu->fasta.title, &curChr);
            }
            pseuPos = offset;
//A counter so the user has some idea of how long it will take
            if (++count % 100 == 0){
                printf ("%d pseudochromosomes assembled...\n", count);
            }
        }
//Invert sequence if needed
        if (bestMatch->chrStrand != bestMatch->scafStrand) {
            invertSequence (curFasta);
            curTitle->inverted = '-';
        }
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
    prevTitle->next = NULL;
    freePseuEntry (curTitle);
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
    return *firFasta;
}

//Adds a string's content to another string
void concatonateString (string *destination, string *source) {
    int i;
    for (i = 0; i < (source->len - 1); i++) {
        readValueToString (destination, source->str[i]);
    }
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

//Find the best match between two blastEntries
void findBestBlast (blastEntry *curBlast, blastEntry *bestMatch) {
//Local variables
    int e1a, e2a, e1b, e2b;
//Check if the two entries are oriented the same direction
    if (((curBlast->chrStrand != curBlast->next->chrStrand) && (curBlast->scafStrand == curBlast->next->scafStrand)) || ((curBlast->chrStrand == curBlast->next->chrStrand) && (curBlast->scafStrand != curBlast->next->scafStrand))) {
        printf ("Stand mismatch in %s.\n", curBlast->scaf.str);
        exit (4);
    }
//If the first entry is a perfect match, but not the second
    if ((curBlast->score.str[1] == '.') && (curBlast->next->score.str[1] != '.')) {
        bestMatch = curBlast;
//If the first entry is not a perfect match, but the second is
    } else if ((curBlast->score.str[1] != '.') && (curBlast->next->score.str[1] == '.')) {
        bestMatch = curBlast->next;
//If both are perfect matches, look at which is a longer match
    } else if ((curBlast->score.str[1] == '.') && (curBlast->next->score.str[1] == '.')) {
//If the second is bigger, use it
        if ((curBlast->scafEnd - curBlast->scafStart) < (curBlast->next->scafEnd - curBlast->next->scafStart)) {
            bestMatch = curBlast->next;
//Otherwise use the first
        } else {
            bestMatch = curBlast;
        }
//If neither is a perfect match
    } else if ((curBlast->score.str[1] != '.') && (curBlast->next->score.str[1] != '.')){
//Get the score values
        sscanf (curBlast->score.str, "%d%*c%d", &e1a, &e1b);
        sscanf (curBlast->next->score.str, "%d%*c%d", &e2a, &e2b);
//Look for the larger b value
        if (e1b < e2b) {
            bestMatch = curBlast->next;
        } else if (e1b > e2b) {
            bestMatch = curBlast;
//If they're the same look for the smaller a value
        } else if (e1b == e2b){
//If the second is smaller, use it
            if (e1a < e2a) {
                bestMatch = curBlast->next;
//Otherwise use the first
            } else {
                bestMatch = curBlast;
            }
        }
    }
    return;
}

//Frees memory allocated to a blastEntry
void freeBlastEntry (blastEntry *oldEntry){
    free (oldEntry->chr.str);
    free (oldEntry->scaf.str);
    free (oldEntry->score.str);
    return;
}

//Frees memory allocated to a fastaEntry
void freeFastaEntry (fastaEntry *oldEntry) {
    free (oldEntry->title.str);
    free (oldEntry->sequence.str);
    return;
}

//Frees memory allocated to a pseuEntry
void freePseuEntry (pseuEntry *oldEntry) {
    freeFastaEntry (&oldEntry->fasta);
    return;
}

//Sets minimum values to a fastaEntry
void initializeFastaEntry (fastaEntry *newFasta) {
    initializeString (&(*newFasta).title);
    initializeString (&(*newFasta).sequence);
    newFasta->next = NULL;
    return;
}

//Sets minimum values to a blastEntry
void initializeBlastEntry (blastEntry *newBlast) {
    initializeString (&(*newBlast).chr);
    initializeString (&(*newBlast).scaf);
    initializeString (&(*newBlast).score);
    newBlast->next = NULL;
    return;
}

//Sets minimum values to a pseuEntry
void initializePseuEntry (pseuEntry *newPseu) {
    initializeFastaEntry (&newPseu->fasta);
    newPseu->start = 0;
    newPseu->end = 0;
    newPseu->inverted = '+';
    newPseu->next = NULL;
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
    int i = 0, j = (curFasta->sequence.len - 1);
    string invSequence;
    initializeString (&invSequence);
//Allocate needed memory
    invSequence.len = curFasta->sequence.len;
    invSequence.str = malloc (invSequence.len);
//Set the null terminator
    invSequence.str[j] = '\0';
    j--;
//Work backwards, inverting as we go
    while (j >= 0) {
        if (curFasta->sequence.str[i] == 'A') {
            invSequence.str[j] = 'T';
        } else if (curFasta->sequence.str[i] == 'C') {
            invSequence.str[j] = 'G';
        } else if (curFasta->sequence.str[i] == 'G') {
            invSequence.str[j] = 'C';
        } else if (curFasta->sequence.str[i] == 'N') {
            invSequence.str[j] = 'N';
        } else if (curFasta->sequence.str[i] == 'T') {
            invSequence.str[j] = 'A';
        }
        j--;
        i++;
    }
//Replace the old sequence with the new one
    copyString (&curFasta->sequence, &invSequence);
    return;
}

//Loads the entries from a BLASTN file into a linked list
void loadBlastList (blastEntry *curBlast, FILE *inFile) {
//Local variables
    char in;
    int count = 1;
//Load the first entry manually
    fscanf (inFile, "%s%d%d%*c%c%s%d%d%*c%c%s%*c", curBlast->chr.str, &curBlast->chrStart, &curBlast->chrEnd, &curBlast->chrStrand, curBlast->scaf.str, &curBlast->scafStart, &curBlast->scafEnd, &curBlast->scafStrand, curBlast->score.str);
    curBlast->chr.len = strlen (curBlast->chr.str) + 1;
    curBlast->scaf.len = strlen (curBlast->scaf.str) + 1;
//Automate the rest
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if ((feof (inFile)) || (ferror (inFile))) {
            break;
        } else {
            fseek (inFile, -1, SEEK_CUR);
//Make a new entry and move the pointer along
            curBlast->next = malloc (sizeof (*curBlast->next));
            initializeBlastEntry (curBlast->next);
            curBlast = curBlast->next;
//Get values from the file (order is always the same);
            fscanf (inFile, "%s%d%d%*c%c%s%d%d%*c%c%s%*c", curBlast->chr.str, &curBlast->chrStart, &curBlast->chrEnd, &curBlast->chrStrand, curBlast->scaf.str, &curBlast->scafStart, &curBlast->scafEnd, &curBlast->scafStrand, curBlast->score.str);
//A counter so the user has some idea of how long it will take
            curBlast->chr.len = strlen (curBlast->chr.str) + 1;
            curBlast->scaf.len = strlen (curBlast->scaf.str) + 1;
            if (++count % 1000 == 0){
                printf ("%d BLASTN entries loaded...\n", count);
            }
        }
    }
    printf ("%d BLASTN entries loaded.  ", --count);
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
    if ((in == '\t') || (in == ' ')) {
        while (in != '\n') {
            in = fgetc (inFile);
        }
    }
//Automate the rest
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
//If it's a new entry, create a new node and load the title
        } else if (in == '>') {
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
            if ((in == '\t') || (in == ' ')) {
                while (in != '\n') {
                    in = fgetc (inFile);
                }
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

//Savea fasta to file
void printFastaEntry (fastaEntry *fasta, FILE *outFile) {
//Local variables
    int location = 0;
//Print the title
    fprintf (outFile, ">%s\n", fasta->title.str);
//Print the sequence
    while (location < (fasta->sequence.len - 1)) {
        fprintf (outFile, "%c", fasta->sequence.str[location++]);
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
