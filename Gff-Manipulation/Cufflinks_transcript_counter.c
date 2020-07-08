/* This program counts the transcripts, exons, and other entries from a Cufflinks gff3.  It takes a gff3 file as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void initializeString (string *newString);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL;
    string type;
    char in;
    int transcriptCount = 0, exonCount = 0, otherCount = 0, count = 0, start = 0, end = 0, length = 0, tlongest = 0, tshortest = 0, tmean = 0, elongest = 0, eshortest = 0, emean = 0;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
//Collect stats
    printf ("Files opened.  Parsing stats...\n");
    initializeString (&type);
    in = fgetc (inFile);
//Automate the rest
    while (1) {
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Skip the location
        while (in != '\t') {
            in = fgetc (inFile);
        }
//Skip the source
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
//Read the type
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&type, in);
            in = fgetc (inFile);
        }
//Read the start and stop
        fscanf (inFile, "%d%d", &start, &end);
        length = end - start;
//Sort by type
        if (strcmp (type.str, "transcript") == 0) {
            if (tshortest == 0) {
               tshortest = length;
            } else {
                if (length < tshortest) {
                    tshortest = length;
                }
            }
            if (tlongest == 0) {
                tlongest = length;
            } else {
                if (length > tlongest) {
                    tlongest = length;
                }
            }
            tmean += length;
        } else if (strcmp (type.str, "exon") == 0) {
            if (eshortest == 0) {
               eshortest = length;
            } else {
                if (length < eshortest) {
                    eshortest = length;
                }
            }
            if (elongest == 0) {
                if (length > elongest) {
                    elongest = length;
                }
            }
            emean += length;
        }
        in = fgetc (inFile);
//Burn the rest of the line
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Increase the counts as they appear
        if (strcmp (type.str, "transcript") == 0) {
            transcriptCount++;
        } else if (strcmp (type.str, "exon") == 0) {
            exonCount++;
        } else {
            otherCount++;
        }
//A counter so the user has some idea of how long it will take
        if (++count % 100000 == 0) {
            printf ("%d entries processed...\n", count);
        }
        reinitializeString (&type);
        in = fgetc (inFile);
    }
//Display the stats
    tmean /= transcriptCount;
    emean /= exonCount;
    printf ("%d entries process.  All stats Parsed. (Count: shortest-longest, mean)\nTranscipts\t%d:\t%d-%d, %d\nExons\t\t%d:\t%d-%d, %d\nOther\t\t%d\n", count, transcriptCount, tshortest, tlongest, tmean, exonCount, eshortest, elongest, emean, otherCount);
//Close everything and free memory
    free (type.str);
    fclose (inFile);
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
