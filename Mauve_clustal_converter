/* This program pulls the data from a Mauve output file and makes a clustalw file out of it.  It takes the unextensioned Mauve output file, the name of the first species therein, and the name of the second species therein as inputs. */

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
typedef struct fastaEntry {
    string title;
    int start;
    int end;
    char strand;
    string sequence;
} fastaEntry;

//List of functions, alphabetically
void createFastaEntry (fastaEntry **firTitle);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void freeFastaEntry (fastaEntry **entry);
void initializeString (string *newString);
void loadFastas (fastaEntry *firEntry, fastaEntry *secEntry, FILE *InFile);
void printFastas (fastaEntry *firEntry, fastaEntry *secEntry, char *species1, char *species2, FILE *outFile);
void readTitle (fastaEntry *entry, FILE *inFile);
void readValueToString (string *string, char in);

//main ()
int main (int argc, char *argv[]) {
//Main variables
    int count = 0;
    char in;
//Call syntax check
    if (argc != 4) {
        printf ("Usage: %s Mauve_comparison_filename Species_1_name Species_2_name\n", argv[0]);
        exit (1);
    }
//Main variables
    fastaEntry *firEntry = NULL, *secEntry = NULL;
    FILE *inFile, *outFile;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Start converting the entries, one at a time
    printf ("Files found and created.  Converting data ...\n");
    while (1) {
//Stop conditions and reset FILE pointer
        in = fgetc (inFile);
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
        fseek (inFile, -1, SEEK_CUR);
//Make from scratch to save memory
        createFastaEntry (&firEntry);
        createFastaEntry (&secEntry);
//Load and print
        loadFastas (firEntry, secEntry, inFile);
        printFastas (firEntry, secEntry, argv[2], argv[3], outFile);
//Free now that they aren't needed
        freeFastaEntry (&firEntry);
        freeFastaEntry (&secEntry);
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0){
            printf ("%d entries converted...\n", count);
        }
    }
    printf ("%d entries counverted\n", count);
//Close everything
    printf ("Converted.  Closing files and freeing memory...\n");
    fclose (inFile);
    fclose (outFile);
    printf ("Done.\n");
    return 0;
}

//Create a new node on a scaffold title list
void createFastaEntry (fastaEntry **newEntry) {
    *newEntry = malloc (sizeof (**newEntry));
    initializeString (&(*newEntry)->title);
    initializeString (&(*newEntry)->sequence);
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
    char *outTitle = "", *fileName = "";
    int i = strlen (inName) - 1, j = 0;
//Check if there is a path given instead of jsut a filename
    while (i > 0) {
        if (inName[i] == '/') {
            i++;
            break;
        }
        i--;
    }
//Pull the filename from the input argument
    fileName = malloc (strlen (inName) - i + 1);
    fileName[0] = '\0';
    while (i < strlen (inName)) {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 5);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, ".clw");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Frees all data alloc'd by createTitleEntry()
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

//Fill a fastaEntry with titles, size, strand, and sequence values
void loadFastas (fastaEntry *firEntry, fastaEntry *secEntry, FILE *inFile) {
//Local variables
    char in = 0;
//Find start point
    while (in != '>') {
        in = fgetc (inFile);
        if (((ferror (inFile)) || (feof (inFile)))) {
            return;
        }
    }
//Read title data
    readTitle (firEntry, inFile);
    in = fgetc (inFile);
//Read sequence data
    while (in != '>') {
        if (in != '\n') {
            readValueToString (&firEntry->sequence, in);
        }
        in = fgetc (inFile);
    }
//Read second title data
    readTitle (secEntry, inFile);
    in = fgetc (inFile);
//Read sequence data
    while (in != '=') {
        if (in != '\n') {
            readValueToString (&secEntry->sequence, in);
        }
        in = fgetc (inFile);
    }
    fgetc (inFile);
    return;
}

//Print the data in a standardized format conforming to the clustalw setup
void printFastas (fastaEntry *firEntry, fastaEntry *secEntry, char *species1, char *species2, FILE *outFile) {
//Local variables
    char *line1 = "", *line2 = "";
    int i, position1 = 0, position2 = 0, compare = 0;
    line1 = malloc (54);
    line2 = malloc (51);
//Load line variables with appropriate data
    sprintf (line1, "%s-%s(%c)/%d-%d", species1, firEntry->title.str, firEntry->strand, firEntry->start, firEntry->end);
    sprintf (line2, "%s-%s(%c)/%d-%d", species2, secEntry->title.str, secEntry->strand, secEntry->start, secEntry->end);
//Set line
    fprintf (outFile, "CLUSTAL W (1.81) multiple sequence alignment\n\n\n");
    while (position1 < (firEntry->sequence.len - 1)) {
//Print first entry
        fprintf (outFile, "%s", line1);
//Add spaces as needed
        for (i = (strlen (line1) - 1); i < 54; i++) {
            fprintf (outFile, " ");
        }
//Print first sequence
        do {
            fprintf (outFile, "%c", firEntry->sequence.str[position1]);
            position1++;
        } while ((position1 % 60 != 0) && (position1 < (firEntry->sequence.len - 1)));
//Printe second entry
        fprintf (outFile, "\n%s", line2);
//Add spaces as needed
        for (i = (strlen (line2) - 1); i < 54; i++) {
            fprintf (outFile, " ");
        }
//Print second sequence
        do {
            fprintf (outFile, "%c", secEntry->sequence.str[position2]);
            position2++;
        } while ((position2 % 60 != 0) && (position2 < (secEntry->sequence.len - 1)));
//Add matching line
        fprintf (outFile, "\n");
//Spaces as needed
        for (i = 0; i < 55; i++) {
            fprintf (outFile, " ");
        }
//Match characters
        do {
            if (firEntry->sequence.str[compare] == secEntry->sequence.str[compare]) {
                fprintf (outFile, "*");
            } else {
                fprintf (outFile, " ");
            }
        } while (++compare < position1);
        fprintf (outFile, "\n\n\n");
    }
//Free the used lines
    free (line1);
    free (line2);
    return;
}

//Read the data from the file for the title, start/end poitns, and strand.  Also, check if there's a bad overlap.
void readTitle (fastaEntry *entry, FILE *inFile) {
    char in = 0;
//One character at a time
    fgetc (inFile);
    in = fgetc (inFile);
    while (in != ':') {
        readValueToString (&entry->title, in);
        in = fgetc (inFile);
    }
//Read the start/end points and strand, then move to next line
    fscanf (inFile, "%d%*c%d%*c%c", &entry->start, &entry->end, &entry->strand);
    in = fgetc (inFile);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
