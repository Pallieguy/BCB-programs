/* This program changes the titles from a tabular BLAST output file to match those of the fasta input.  It takes the SMFasta_title_parser output from the fasta used as the index and the tabular BLAST (qseid,sseqid,...) results as inputs. */

//Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//List of fasta titles
typedef struct titleEntry {
    string match;
    string title;
    struct titleEntry *next;
} titleEntry;

//List of functions, alphabetically
void correctTitle (titleEntry *firTitle, FILE *inFile, FILE *outFile);
void createFile (FILE **file, char *fName, char perm);
void createTitleEntry (titleEntry **newTitle);
void createOutputFile (FILE **outFile, char *inName);
void freeTitleEntry (titleEntry **title);
void freeTitleList (titleEntry *firTitle);
void initializeString (string *newString);
void loadTitles (titleEntry *firTitle, FILE *inFile);
void readValueToString (string *string, char in);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Title_list_filename BLAST_results_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    titleEntry *firTitle = NULL;
    FILE *tInFile, *bInFile, *outFile;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&tInFile, argv[1], 'r');
    createFile (&bInFile, argv[2], 'r');
    createOutputFile (&outFile, argv[2]);
//Build dynamic linked list of the first scaffold titles
    printf ("Files found and created.  Compiling fasta titles...\n");
    createTitleEntry (&firTitle);
    loadTitles (firTitle, tInFile);
    fclose (tInFile);
//Check each comparison entry and correct the title
    printf ("  Correcting BLAST titles...\n");
    correctTitle (firTitle, bInFile, outFile);
//Close everything
    printf ("  Freeing memory and closing files...\n");
    fclose (bInFile);
    fclose (outFile);
    freeTitleList (firTitle);
    printf ("Done.\n");
    return 0;
}

//Replace the number with to appropriate fasta title
void correctTitle (titleEntry *firTitle, FILE *inFile, FILE *outFile) {
//Local variables
    titleEntry *curTitle = NULL;
    char in;
    string match;
    int count = 1;
//Loop through the BLAST file
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//initialize local variables
        curTitle = firTitle;
        initializeString (&match);
//Copy the qseqid
        while (in != '\t') {
            fprintf (outFile, "%c", in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Read the sseqid
        while (in != '\t') {
            readValueToString (&match, in);
            in = fgetc (inFile);
        }
//Find the corresponding sseqid from the match list
        while (strcmp (curTitle->match.str, match.str) != 1) {
            curTitle = curTitle->next;
            if (curTitle == NULL) {
                printf ("Error! %s not found in fasta!\n", match.str);
                exit (1);
            }
        }
//Print the long form title
        fprintf (outFile, "\t%s %s", curTitle->match.str, curTitle->title.str);
//Copy the rest of the BLAST data
        while (in != '\n') {
            fprintf (outFile, "%c", in);
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Add a newline if there's more left
        if (!((ferror (inFile)) || (feof (inFile)))) {
            fprintf (outFile, "\n");
        }
//Empty the current match
        free (match.str);
        match.len = 0;
//A counter so the user has some idea of how long it will take
        if (++count % 100 == 0) {
            printf ("%d entries corrected...\n", count);
        }
    }
    printf ("%d entries corrected.", count);
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
    initializeString (&(*newTitle)->match);
    initializeString (&(*newTitle)->title);
    (*newTitle)->next = NULL;
    return;
}

//Frees all data alloc'd by createTitleEntry()
void freeTitleEntry (titleEntry **title) {
    free ((*title)->match.str);
    free ((*title)->title.str);
    free (*title);
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

//Fill a DLL with titles and size values
void loadTitles (titleEntry *firTitle, FILE *inFile) {
//Local variables
    titleEntry *curTitle = NULL, *prevTitle = NULL;
    int start = 1, count = 1;
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
//Read the match
        while (in != ' ') {
            readValueToString (&curTitle->match, in);
            in = fgetc (inFile);
        }
//Read the title to the next tab or newline
        in = fgetc (inFile);
        while ((in != '\t') && (in != '\n')) {
            readValueToString (&curTitle->title, in);
            in = fgetc (inFile);
        }
//Skip the rest of the title line
        while (in != '\n') {
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Move to next node
        createTitleEntry (&curTitle->next);
        prevTitle = curTitle;
        curTitle = curTitle->next;
//A counter so the user has some idea of how long it will take
        if (++count % 100000 == 0) {
            printf ("%d entries compiled...\n", count);
        }
    }
//Free the un-need node
    printf ("%d entries compiled.", count);
    prevTitle->next = NULL;
    freeTitleEntry (&curTitle);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
