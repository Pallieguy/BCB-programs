/* This program interleaves two title into a single file.  It takes two title lists as input.  I suspect this is a one off program for the Chr08 interleaving.*/

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Standard data from a fasta entry
typedef struct titleEntry {
    string scaffold;
    int start;
    int end;
    int size;
    char strand;
    struct titleEntry *next;
} titleEntry;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void freeTitleEntry (titleEntry *oldEntry);
void freeTitleEntryList (titleEntry *firEntry);
void initializeTitleEntry (titleEntry *newEntry);
void initializeString (string *newString);
void interleaveTitles (titleEntry *firEntry1, titleEntry *firEntry2, FILE *outFile);
void loadTitleEntryList (titleEntry *firEntry, FILE *inFile);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s First_title_filename Second_title_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    int count = 0;
    FILE *inFile1 = NULL, *inFile2 = NULL, *outFile = NULL;
    titleEntry firEntry1, firEntry2;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile1, argv[1], 'r');
    createFile (&inFile2, argv[2], 'r');
    createOutputFile (&outFile, "OAC-Rex_titles.txt");
//Load the entries from the first list
    printf ("Files opened.  Loading first titles list...\n");
    initializeTitleEntry (&firEntry1);
    loadTitleEntryList (&firEntry1, inFile1);
    fclose (inFile1);
//Load the entries from the second list
    printf ("Loading second titles list...\n");
    initializeTitleEntry (&firEntry2);
    loadTitleEntryList (&firEntry2, inFile2);
    fclose (inFile2);
//Interleave the titles in ascending order by their start positions
    printf ("Interleaving titles...\n");
    interleaveTitles (&firEntry1, &firEntry2, outFile);
//Close everything
    printf ("Closing files and freeing memory...\n");
    freeTitleEntryList (&firEntry1);
    freeTitleEntryList (&firEntry2);
    fclose (outFile);
    printf ("Done.\n");
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
    while (inName[i] != '\0') {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 12);
    outTitle[0] = '\0';
    strcat (outTitle, "interleaved_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Frees the memory used by a fastaEntry node
void freeTitleEntry (titleEntry *oldEntry) {
    free (oldEntry->scaffold.str);
    free (oldEntry->next);
    return;
}

//Frees a list of titleEntrys
void freeTitleEntryList (titleEntry *firEntry) {
    titleEntry *curEntry = NULL;
    while (firEntry != NULL) {
        curEntry = firEntry->next;
        freeTitleEntry (firEntry);
        free (firEntry);
        firEntry = curEntry;
    }
    return;
}

//Sets the minimum values to a fastaEntry
void initializeTitleEntry (titleEntry *newEntry) {
    initializeString (&newEntry->scaffold);
    newEntry->start = 0;
    newEntry->end = 0;
    newEntry->size = 0;
    newEntry->strand = ' ';
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

//Interleaves the lists together in ascending order
void interleaveTitles (titleEntry *firEntry1, titleEntry *firEntry2, FILE *outFile) {

    return;
}

//Finds the next entry in a file, sets the string to NULL at EOF
void loadTitleEntryList (titleEntry *firEntry, FILE *inFile) {
//Local variables
    char in;
    int count = 0;
    titleEntry *curEntry = NULL, *prevEntry = NULL;
//Find the first entry
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
    curEntry = firEntry;
    prevEntry = firEntry;
//Loop the rest
    while (1) {
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Read the scaffold
        in = fgetc (inFile);
        while ((in != ',') && (in != ' ') && (in != '\t') && (in != '\n')) {
            readValueToString (&curEntry->scaffold, in);
            in = fgetc (inFile);
        }
//Read start, end, size, and strand
        fscanf (inFile, "%d%*c%d%*c%d%*c%c", &curEntry->start, &curEntry->end, &curEntry->size, &curEntry->strand);
printf ("%s,%d,%d,%d,%c\n", curEntry->scaffold.str, curEntry->start, curEntry->end, curEntry->size, curEntry->strand);
//Burn the rest of the line
        in = fgetc (inFile);
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Move to the next entry on the list
        initializeTitleEntry (curEntry->next);
        prevEntry = curEntry;
        curEntry = curEntry->next;        
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0) {
            printf ("%d titles loaded...\n", count);
        }
    }
//Remove the empty entry
    freeTitleEntry (curEntry);
    prevEntry->next = NULL;
    printf ("%d titles loaded.  ", count);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}

//Resets a used string to an empty status
void reinitializeString (string *string) {
    free (string->str);
    initializeString (&(*string));
    return;
}
