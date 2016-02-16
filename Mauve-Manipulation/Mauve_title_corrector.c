/* This program changes the titles from a Mauve output file to match those of the multi-fasta inputs.  It takes the SMFasta_title_parser output from the first Mauve input source, the SMFasta_title_parser output from the second Mauve input source, and the unextensioned Mauve output file as inputs. */

//Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Entry for the title and size variables
typedef struct titleEntry {
    string title;
    int start;
    int end;
    struct titleEntry *next;
} titleEntry;

//Entry for the sequence itself
typedef struct fastaEntry {
    string title;
    int start;
    int end;
    int errorCount;
    int errorLength;
    char strand;
    string sequence;
    struct fastaEntry *next;
} fastaEntry;

//List of functions, alphabetically
void correctMauveTitle (titleEntry *firTitle, fastaEntry *entry, FILE *inFile);
void createFastaEntry (fastaEntry **firTitle);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void createTitleEntry (titleEntry **firTitle);
void doubleOverlap (fastaEntry *firEntry, fastaEntry *secEntry);
void freeFastaEntry (fastaEntry **entry);
void freeTitleEntry (titleEntry **title);
void freeTitleList (titleEntry *firTitle);
void initializeString (string *newString);
void insertFasta (fastaEntry *longEntry, int shortSize, int longSize);
void lengthCheck (fastaEntry *entry, titleEntry *curTitle);
void loadError (fastaEntry *entry, titleEntry *curTitle, fastaEntry *check);
void loadTitles (titleEntry *firTitle, FILE *InFile);
void outputFastaEntries (fastaEntry *firEntry, fastaEntry *secEntry, FILE *outFile);
void pathCheck (fastaEntry *firEntry, fastaEntry *secEntry);
void printFastaEntries (fastaEntry *firEntry, fastaEntry *secEntry, FILE *outFile);
void printFastaEntry (fastaEntry *entry, FILE *outFile);
void readValueToString (string *string, char in);
void sortEntries (titleEntry *firTitle, titleEntry *secTitle, FILE *inFile, FILE *outFile);
void stringCopy (string *dest, string *source);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 4) {
        printf ("Usage: %s First_title_list_filename Second_title_list_filename Mauve_comparison_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    titleEntry *firTitle = NULL, *secTitle = NULL;
    FILE *fInFile, *sInFile, *mInFile, *outFile;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&fInFile, argv[1], 'r');
    createFile (&sInFile, argv[2], 'r');
    createFile (&mInFile, argv[3], 'r');
    createOutputFile (&outFile, argv[3]);
//Build dynamic linked list of the first scaffold titles
    printf ("Files found and created.  Compiling first set of scaffold titles...\n");
    createTitleEntry (&firTitle);
    loadTitles (firTitle, fInFile);
    fclose (fInFile);
//Build dynamic linked list of the second scaffold titles
    printf ("Compiled.  Compiling second set of scaffold titles...\n");
    createTitleEntry (&secTitle);
    loadTitles (secTitle, sInFile);
    fclose (sInFile);
//Check each comparison entry and correct the title
    printf ("Compiled.  Correcting Mauve titles...\n");
    sortEntries (firTitle, secTitle, mInFile, outFile);
//Close everything
    printf ("  Freeing memory and closing files...\n");
    fclose (mInFile);
    fclose (outFile);
    freeTitleList (firTitle);
    freeTitleList (secTitle);
    printf ("Done.\n");
    return 0;
}

//Replace the number with to appropriate fasta title
void correctMauveTitle (titleEntry *firTitle, fastaEntry *entry, FILE *inFile) {
//Local variables
    titleEntry *curTitle = NULL;
    char in;
    curTitle = firTitle;
//Find the `:` then read the start/stop points and strand, burn the rest
    in = fgetc (inFile);
    while (in != ':') {
        in = fgetc (inFile);
    }
    fscanf (inFile, "%d%*c%d%*c%c%*[^\n]%*c", &entry->start, &entry->end, &entry->strand);
//Find the right title
    while (entry->start > curTitle->end) {
        curTitle = curTitle->next;
    }
//Print correct title
    stringCopy (&entry->title, &curTitle->title);
    entry->start -= (curTitle->start - 1);
    entry->end -= (curTitle->start - 1);
//Collect sequence data
    in = fgetc (inFile);
    while ((in != '>') && (in != '=')) {
        if (in != '\n') {
            readValueToString (&entry->sequence, in);
        }
        in = fgetc (inFile);
    }
//Check the length compared to the end point
    lengthCheck (entry, curTitle);
    return;
}

//Create a new node on a scaffold title list
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
/*//Remove the file extention.  EDIT THIS AT COPY
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
    outTitle = malloc (j + 11);
    outTitle[0] = '\0';
    strcat (outTitle, "corrected_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Create a new node on a scaffold title list
void createTitleEntry (titleEntry **newTitle) {
    *newTitle = malloc (sizeof (**newTitle));
    initializeString (&(*newTitle)->title);
    (*newTitle)->next = NULL;
    return;
}

//Special handling for rare event of a double overlap wherein both are long enough to keep
void doubleOverlap (fastaEntry *firEntry, fastaEntry *secEntry) {
//Local variables
    int size1 = firEntry->end - firEntry->start, size2 = secEntry->end - secEntry->start, i;
//Change the end point of the longer scaffold to match the shorter
    if (size1 < size2) {
        insertFasta (secEntry, size1, size2);
    } else if (size1 > size2) {
        insertFasta (firEntry, size2, size1);
    }
//Potential recursive check how to proceed
    pathCheck (firEntry->next, secEntry->next);
    return;
}

//Frees all data alloc'd by createTitleEntry()
void freeFastaEntry (fastaEntry **entry) {
    free ((*entry)->title.str);
    free ((*entry)->sequence.str);
    free (*entry);
    return;
}

//Frees all data alloc'd by createTitleEntry()
void freeTitleEntry (titleEntry **Title) {
    free ((*Title)->title.str);
    free (*Title);
    return;
}

//Frees a list of titleEntries
void freeTitleList (titleEntry *firTitle) {
    titleEntry *prevTitle = NULL;
    while (firTitle != NULL) {
        prevTitle = firTitle;
        firTitle = firTitle->next;
        freeTitleEntry (&prevTitle);
    }
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Cut a fasta entry that is too long and insert a new entry into the list if the remainder is big enough
void insertFasta (fastaEntry *longEntry, int shortSize, int longSize) {
//Local variables
    int i;
    fastaEntry *newEntry = NULL;
    createFastaEntry (&newEntry);
//Copy the name and end
    stringCopy (&newEntry->title, &longEntry->title);
    newEntry->end = longEntry->end;
//Find the new end of the first entry
    longEntry->end = longEntry->start + shortSize;
//Calaculate the other stats for the new entry
    newEntry->start = longEntry->end + 1;
    newEntry->errorCount = longSize - shortSize;
    newEntry->errorLength = newEntry->end - newEntry->errorCount;
//Find how many characters correspond to the count
    for (i = 1; i <= newEntry->errorLength; i++) {
        if (longEntry->sequence.str[(longEntry->sequence.len - i)] == '-') {
            newEntry->errorLength++;
        }
    }
//Only do the big work if it's big enough
    if ((longSize - shortSize) > 50) {
//Copy the sequence
        for (i = ((longEntry->sequence.len - newEntry->errorLength) - 1); i < longEntry->sequence.len; i++) {
        readValueToString (&newEntry->sequence, longEntry->sequence.str[i]);
        }
//Insert into the DLL
        newEntry->next = longEntry->next;
        longEntry->next = newEntry;
    }
//Adjust the original sequence either way
    longEntry->sequence.len -= newEntry->errorLength;
    longEntry->sequence.str[(longEntry->sequence.len -1)] = '\0';
//Delete the new info if it's not big enough to insert
    if ((longSize - shortSize) < 51) {
        freeFastaEntry (&newEntry);
    }
    return;
}

//Check if there's an overlap, if so, split the overlaps up
void lengthCheck (fastaEntry *entry, titleEntry *curTitle) {
//Local variables
    int i;
//Only work if there is overlap
    if (entry->end > curTitle->end) {
        entry->errorCount = (entry->end - curTitle->end);
        entry->errorLength = entry->errorCount;
//Account for any `-` in the error segment
        for (i = 1; i <= entry->errorLength; i++) {
            if (entry->sequence.str[(entry->sequence.len - i)] == '-') {
                entry->errorLength++;
            }
        }
//Add if the overlap is big enough and check that the new one doesn't overlap as well
        if (entry->errorCount >= 50) {
            loadError (entry, curTitle, NULL);
            lengthCheck (entry->next, curTitle);
//If it's not big enough, jsut delete the overlap
        } else {
            entry->end = curTitle->end;
            entry->sequence.len -= entry->errorLength;
            entry->sequence.str[(entry->sequence.len - 1)] = '\0';
        }
    }
    return;
}

//Correct the overlapping data
void loadError (fastaEntry *entry, titleEntry *curTitle, fastaEntry *check) {
//Local variables
    int i;
    fastaEntry *newError = NULL;
    createFastaEntry (&entry->next);
    newError = entry->next;
//If the data is an overlap
    if (curTitle != NULL) {
        curTitle = curTitle->next;
        stringCopy (&newError->title, &curTitle->title);
        newError->start = 1;
        newError->end = entry->errorCount;
//If it is not an overlap
    } else {
        stringCopy (&newError->title, &entry->title);
        newError->start = (entry->end - entry->errorCount);
        newError->end = entry->end;
    }
//Overlap doesn't change these
    entry->end -= entry->errorCount;
    newError->strand = entry->strand;
//Copy the sequence
    for (i = ((entry->sequence.len - entry->errorLength) - 1); i < (entry->sequence.len - 1); i++) {
        readValueToString (&newError->sequence, entry->sequence.str[i]);
    }
//Update the original sequence endpoint
    entry->sequence.len -= entry->errorCount;
    entry->sequence.str[entry->sequence.len] = '\0';
//Recursive check for multiple splits of a non-overlap
    if ((check != NULL) && (check->next != NULL)) {
        newError->errorCount = check->errorCount;
        newError->errorLength = check->errorLength;
        loadError (entry->next, NULL, check->next);
    }
    return;
}


//Fill a DLL with titles and size values
void loadTitles (titleEntry *firTitle, FILE *inFile) {
//Local variables
    titleEntry *curTitle = NULL, *prevTitle = NULL;
    int start = 1;
    char in;
    curTitle = firTitle;
//One character at a time
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Skip the > if need be
        if (in == '>') {
            in = fgetc (inFile);
        }
//Read the title
        while ((in != '\t') && (in != '\n')) {
            readValueToString (&curTitle->title, in);
            in = fgetc (inFile);
        }
//Skip the rest of the title line
        if (in == '\t') {
            while (in != '\n') {
                in = fgetc (inFile);
            }
        }
//Read the start/end point
        in = fgetc (inFile);
        curTitle->end = start;
        while (in != '>') {
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            if (in != '\n') {
                curTitle->end++;
            }
            in = fgetc (inFile);
        }
        curTitle->start = start;
        start = curTitle->end + 1;
//Move to next node
        createTitleEntry (&curTitle->next);
        prevTitle = curTitle;
        curTitle = curTitle->next;
    }
//Free the un-need node
    prevTitle->next = NULL;
    freeTitleEntry (&curTitle);
    return;
}

//Prints the contents of an entry in a Mauve-esque format
void outputFastaEntries (fastaEntry *firEntry, fastaEntry *secEntry, FILE *outFile) {
//Sort the entries
    pathCheck (firEntry, secEntry);
//Print them after sorting
    printFastaEntries (firEntry, secEntry, outFile);
    return;
}

//Check which sorting path to take
void pathCheck (fastaEntry *firEntry, fastaEntry *secEntry) {
//If only the first entry has the overlap
    if ((firEntry->next != NULL) && (secEntry->next == NULL)) {
        secEntry->errorCount = firEntry->errorCount;
        secEntry->errorLength = firEntry->errorLength;
        loadError (secEntry, NULL, firEntry->next);
//If only the second entry has the overlap
    } else if ((firEntry->next == NULL) && (secEntry->next != NULL)) {
        firEntry->errorCount = secEntry->errorCount;
        firEntry->errorLength = secEntry->errorLength;
        loadError (firEntry, NULL, secEntry->next);
//If they both have overlap
    } else if ((firEntry->next != NULL) && (secEntry->next != NULL)) {
        doubleOverlap (firEntry, secEntry);
    }
    return;
}

//Prints the data from the fastaEntries to file
void printFastaEntries (fastaEntry *firEntry, fastaEntry *secEntry, FILE *outFile) {
//Local variables
    int i;
    fastaEntry *firPrev = NULL, *secPrev = NULL;
//Go through the list (if there are multiples)
    while (firEntry != NULL) {
//Print first entry
        printFastaEntry (firEntry, outFile);
        fprintf (outFile, "\n");
//Print second entry
        printFastaEntry (secEntry, outFile);
        fprintf (outFile, "\n=\n");
//Move down the list
        firPrev = firEntry;
        secPrev = secEntry;
        firEntry = firEntry->next;
        secEntry = secEntry->next;
//Free the printed ones
        freeFastaEntry (&firPrev);
        freeFastaEntry (&secPrev);
    }
    return;
}

//Do the actual printing
void printFastaEntry (fastaEntry *entry, FILE *outFile) {
    int i;
    fprintf (outFile, "> %s:%d-%d %c", entry->title.str, entry->start, entry->end, entry->strand);
    for (i = 0; i < (entry->sequence.len - 1); i++) {
        if ((i % 80) == 0) {
            fprintf (outFile, "\n");
        }
        fprintf (outFile, "%c", entry->sequence.str[i]);
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

//Copy the text from the Mauve output, correcting as needed
void sortEntries (titleEntry *firTitle, titleEntry *secTitle, FILE *inFile, FILE *outFile) {
//Local variables
    char in;
    int count = 0;
    fastaEntry *firEntry = NULL, *secEntry = NULL;
//Go one char at a time
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Copy comment lines verbatum
        if (in == '#') {
            while (in != '\n') {
                fprintf (outFile, "%c", in);
                in = fgetc (inFile);
            }
            fprintf (outFile, "\n");
        }
//if it's an entry load a fasta object
        if (in == '>') {
            createFastaEntry (&firEntry);
            createFastaEntry (&secEntry);
//Load the title and sequence as needed
            correctMauveTitle (firTitle, firEntry, inFile);
            correctMauveTitle (secTitle, secEntry, inFile);
//Correct and print the corrected titles
            outputFastaEntries (firEntry, secEntry, outFile);
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0){
                printf ("%d titles corrected...\n", count);
            }
        }
    }
//A total count
    printf ("%d titles corrected.", count);
    return;
}

//Replaces the values of one string with another
void stringCopy (string *destination, string *source) {
    int i;
    for (i = 0; i < source->len; i++) {
        readValueToString (destination, source->str[i]);
    }
    return;
}
