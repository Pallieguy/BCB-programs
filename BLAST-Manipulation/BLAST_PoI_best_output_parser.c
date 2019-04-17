/* This program filters a BLAST output based on highest bitscore and evalue, keeping ties.  It takes a tsv output file from SMPOIscript as input, the bitscore must be the last column and e-value the second last */

//Standard includes, alphabetically
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Blast control made easy
typedef struct blastEntry {
    string query;
    string target;
    int start;
    int stop;
    int e;
    int value;
    int bitscore;
} blastEntry;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void filterEntries (FILE *inFile, FILE *outFile);
void initializeBlastEntry (blastEntry *newEntry);
void initializeString (string *newString);
void readValueToString (string *string, char in);

//main()
int main (int argc, char *argv[]) {
//Call syntax checks
    if (argc != 2) {
        printf ("Usage: %s BLAST_output_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Do the thing
    printf ("Files found and created.  Filtering entries...\n");
    filterEntries (inFile, outFile);
//Close everything
    printf ("  Closing files and freeing memory...\n");
    fclose (outFile);
    fclose (inFile);
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
    while (i < strlen (inName)) {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 10);
    outTitle[0] = '\0';
    strcat (outTitle, "filtered_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Fitler out the lower quality matches
void filterEntries (FILE *inFile, FILE *outFile) {
//Local variables
    char in, e;
    int count = 0;
    blastEntry *prevQuery = NULL, *curQuery = NULL;
//Loop it all
    while (1) {
//Load the current entry
        curQuery = malloc (sizeof (*curQuery));
        initializeBlastEntry (curQuery);
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Query
        while (in != '\t') {
            readValueToString (&curQuery->query, in);
            in = fgetc (inFile);
        }
//Target
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&curQuery->target, in);
            in = fgetc (inFile);
        }
//Start/Stop
        fscanf (inFile, "%d%d", &curQuery->start, &curQuery->stop);
//e
        in = fgetc (inFile);
        in = fgetc (inFile);
        curQuery->e = atoi (&in);
        in = fgetc (inFile);
        in = fgetc (inFile);
//scores
        fscanf (inFile, "%d%d", &curQuery->value, &curQuery->bitscore);
//Prep for the next entry
        while (in != '\n') {
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Compare to the last entry
        if (prevQuery != NULL) {
//If it's a new query, print the free the old one
            if (strcmp (curQuery->query.str, prevQuery->query.str) != 0) {
                fprintf (outFile, "%s\t%s\t%d\t%d\t%de-%d\t%d\n", prevQuery->query.str, prevQuery->target.str, prevQuery->start, prevQuery->stop, prevQuery->e, prevQuery->value, prevQuery->bitscore);
                free (prevQuery->query.str);
                free (prevQuery->target.str);
                free (prevQuery);
                prevQuery = curQuery;
//If it's the same query check the bitscore, keep the higher
            } else if (prevQuery->bitscore != curQuery->bitscore) {
                free (curQuery->query.str);
                free (curQuery->target.str);
                free (curQuery);
//If the bitscores are identical check the evalue, keep only if they are identical
            } else if (prevQuery->value == curQuery->value) {
                if (prevQuery->e == curQuery->e) {
                    if ((prevQuery->start != curQuery->start) || (prevQuery->stop != curQuery->stop)) {
                        fprintf (outFile, "%s\t%s\t%d\t%d\t%de-%d\t%d\n", curQuery->query.str, curQuery->target.str, curQuery->start, curQuery->stop, curQuery->e, curQuery->value, curQuery->bitscore);
                    }
                }
                free (curQuery->query.str);
                free (curQuery->target.str);
                free (curQuery);
            }
        } else {
            prevQuery = curQuery;
        }
        curQuery = NULL;
//A counter so the user has some idea of time frame.
        if (++count % 10000 == 0) {
            printf ("%d entries sorted...\n", count);
        }
    }
    fprintf (outFile, "%s\t%s\t%d\t%d\t%de-%d\t%d\n", prevQuery->query.str, prevQuery->target.str, prevQuery->start, prevQuery->stop, prevQuery->e, prevQuery->value, prevQuery->bitscore);
    free (prevQuery->query.str);
    free (prevQuery->target.str);
    free (prevQuery);
    printf ("%d entries sorted.", ++count);
    return;
}

//Sets minimum values to a blastEntry
void initializeBlastEntry (blastEntry *newEntry) {
    initializeString (&newEntry->query);
    initializeString (&newEntry->target);
    newEntry->e = 0;
    newEntry->value = 0;
    newEntry->bitscore = 0;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[((string->len) - 2)] = in;
    string->str[((string->len) - 1)] = '\0';
    return;
}
