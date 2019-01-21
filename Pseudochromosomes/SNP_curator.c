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


//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void initializeBlastEntry (blastEntry *newEntry);
void initializeString (string *newString);
void loadBlastList (blastEntry *firEntry, FILE *inFile);
void printBlastList (blastEntry *firEntry, FILE *outFile);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 2) {
        printf ("Usage: %s SNP_blast_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    blastEntry *firEntry = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argV[1], 'r');
    createOutputFile (&outFile, argV[1]);
//Load BLAST entries
    printf ("Loading SNP entries...\n");
    firEntry = malloc (sizeof (*firEntry));
    initializeBlastEntry (firEntry);
    loadBlastList (firEntry, inFile);
    fclose (inFile);
//Print the list
    printf ("  Printing curated list...\n");
    printBlastList (firEntry, outFile);
//Close everything and free memory
    printf ("Done.  Closing files and freeing memory...\n");
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
    outTitle = malloc (j + 9);
    outTitle[0] = '\0';
    strcat (outTitle, "curated_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Sets the minimum vlaues to a BLAST entry
void initializeBlastEntry (blastEntry *newEntry) {
    initializeString (&(*newEntry).chr);
    initializeString (&(*newEntry).scaf);
    initializeString (&(*newEntry).evalue);
    newEntry->next = NULL;
    return;}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Loads all the data from the BLAST file to memory
void loadBlastList (blastEntry *firEntry, FILE *inFile) {
//Local variables
    char in;
    int count = 1, curVal, curExp, newVal, newExp;
    blastEntry *curEntry = firEntry, *prevEntry = NULL, *newEntry = NULL;
//Load the first entry
//Load chr
    in = fgetc (inFile);
    while (in != '_') {
        readValueToString (&curEntry->chr, in);
        in = fgetc (inFile);
    }
//Load chrStart and chrEnd
    fscanf (inFile, "%d%*c%d%*c", &curEntry->chrStart, &curEntry->chrEnd);
//load chrStrand
    curEntry->chrStrand = fgetc (inFile);
    while (in != '\t') {
        in = fgetc (inFile);
    }
//Load scaf
    in = fgetc (inFile);
    while (in != '\t') {
        readValueToString (&curEntry->scaf, in);
        in = fgetc (inFile);
    }
//Load scafStart and scafEnd
    fscanf (inFile, "%d%d", &curEntry->scafStart, &curEntry->scafEnd);
//Load scafStrand
    in = fgetc (inFile);
    in = fgetc (inFile);
    if (in == 'm') {
        curEntry->scafStrand = '-';
    } else {
        curEntry->scafStrand = '+';
    }
    while (in != '\t') {
        in = fgetc (inFile);
    }
//load evalue
    in = fgetc (inFile);
    while (in != '\n') {
        readValueToString (&curEntry->evalue, in);
        in = fgetc (inFile);
    }
//Loop the loading/sorting process
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Reset the search
        curEntry = firEntry;
        prevEntry = NULL;
        newEntry = malloc (sizeof (*newEntry));
        initializeBlastEntry (newEntry);
//load the new entry
//Load chr
        while (in != '_') {
            readValueToString (&newEntry->chr, in);
            in = fgetc (inFile);
        }
//Load chrStart and chrEnd
        fscanf (inFile, "%d%*c%d%*c", &newEntry->chrStart, &newEntry->chrEnd);
//load chrStrand
        newEntry->chrStrand = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
//Load scaf
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&newEntry->scaf, in);
            in = fgetc (inFile);
        }
//Load scafStart and scafEnd
        fscanf (inFile, "%d%d", &newEntry->scafStart, &newEntry->scafEnd);
//Load scafStrand
        in = fgetc (inFile);
        in = fgetc (inFile);
        if (in == 'm') {
            newEntry->scafStrand = '-';
        } else {
            newEntry->scafStrand = '+';
        }
        while (in != '\t') {
            in = fgetc (inFile);
        }
//load evalue
        in = fgetc (inFile);
        while (in != '\n') {
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            readValueToString (&newEntry->evalue, in);
            in = fgetc (inFile);
        }
//Look through the list for scaffold matches for any matching scafs
        while ((curEntry != NULL) && (strcmp (curEntry->scaf.str, newEntry->scaf.str) != 0)) {
            prevEntry = curEntry;
            curEntry = curEntry->next;
        }
//If it's a new scaf add it to the list
        if (curEntry == NULL) {
            prevEntry->next = newEntry;
            newEntry = NULL;
//If it's not a new scaff find the best scaf
        } else {
//If the old entry is a perfect match, but not the new one, keep the old one
            if ((curEntry->evalue.str[1] == '.') && (newEntry->evalue.str[1] != '.')) {
                free (newEntry->chr.str);
                free (newEntry->scaf.str);
                free (newEntry->evalue.str);
                free (newEntry);
//If the old entry is not a perfect match, but the new one is, take the new one
            } else if ((curEntry->evalue.str[1] != '.') && (newEntry->evalue.str[1] == '.')) {
//If it's replacing the first Entry
                if (prevEntry == NULL) {
                    newEntry->next = firEntry->next;
                    firEntry = newEntry;
//If it's replacing anything else
                } else {
                    prevEntry->next = newEntry;
                    newEntry->next = curEntry->next;
                }
//Free the old one
                free (curEntry->chr.str);
                free (curEntry->scaf.str);
                free (curEntry->evalue.str);
                free (curEntry);
//If both are perfect matches, look at which is a longer match
            } else if ((curEntry->evalue.str[1] == '.') && (newEntry->evalue.str[1] == '.')) {
//If the old one is shorter, replace it
                if ((curEntry->scafEnd - curEntry->scafStart) < (newEntry->scafEnd - newEntry->scafStart)) {
//If it's replacing the first Entry
                    if (prevEntry == NULL) {
                        newEntry->next = firEntry->next;
                        firEntry = newEntry;
//If it's replacing anything else
                    } else {
                        prevEntry->next = newEntry;
                        newEntry->next = curEntry->next;
                    }
//Free the old one
                    free (curEntry->chr.str);
                    free (curEntry->scaf.str);
                    free (curEntry->evalue.str);
                    free (curEntry);
//Otherwise keep the old one
                } else {
                    free (newEntry->chr.str);
                    free (newEntry->scaf.str);
                    free (newEntry->evalue.str);
                    free (newEntry);
                }
//If neither is a perfect match, Look for the lower evalue exponent value
            } else {
//Get the score values
                sscanf (curEntry->evalue.str, "%d%*c%d", &curVal, &curExp);
                sscanf (newEntry->evalue.str, "%d%*c%d", &newVal, &newExp);
//If current is lower, keep it
                if (curExp < newExp) {
                    free (newEntry->chr.str);
                    free (newEntry->scaf.str);
                    free (newEntry->evalue.str);
                    free (newEntry);
//If the new exponent is lower, replace current
                } else if (curExp > newExp) {
//If it's replacing the first Entry
                    if (prevEntry == NULL) {
                        newEntry->next = firEntry->next;
                        firEntry = newEntry;
//If it's replacing anything else
                    } else {
                        prevEntry->next = newEntry;
                        newEntry->next = curEntry->next;
                    }
//Free the old one
                    free (curEntry->chr.str);
                    free (curEntry->scaf.str);
                    free (curEntry->evalue.str);
                    free (curEntry);
//If they're the same look for the smaller a value
                } else if (curExp == newExp) {
//If the new one is larger, use it
                    if (curVal < newVal) {
//If it's replacing the first Entry
                        if (prevEntry == NULL) {
                            newEntry->next = firEntry->next;
                            firEntry = newEntry;
//If it's replacing anything else
                        } else {
                            prevEntry->next = newEntry;
                            newEntry->next = curEntry->next;
                        }
//Free the old one
                        free (curEntry->chr.str);
                        free (curEntry->scaf.str);
                        free (curEntry->evalue.str);
                        free (curEntry);
//Otherwise keep the old one
                    } else {
                        free (newEntry->chr.str);
                        free (newEntry->scaf.str);
                        free (newEntry->evalue.str);
                        free (newEntry);
                    }
                }
            }
        }
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0){
            printf ("%d blast entries assessed...\n", count);
        }
    }
    printf ("%d blast entries assessed.", count);
    return;
}

//Prints the curated BLAST entries to file
void printBlastList (blastEntry *firEntry, FILE *outFile) {
//Local Variables
    blastEntry *curEntry = firEntry;
    int count = 0;
//Loop the process
    while (curEntry != NULL) {
        fprintf (outFile, "%s\t%d\t%d\t%c\t%s\t%d\t%d\t%c\t%s\n", curEntry->chr.str, curEntry->chrStart, curEntry->chrEnd, curEntry->chrStrand, curEntry->scaf.str, curEntry->scafStart, curEntry->scafEnd, curEntry->scafStrand, curEntry->evalue.str);
        curEntry = curEntry->next;
        free (firEntry->chr.str);
        free (firEntry->scaf.str);
        free (firEntry->evalue.str);
        free (firEntry);
        firEntry = curEntry;
    }
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
