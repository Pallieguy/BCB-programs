/* This program corrects the output from an assembler that changed all 'N' to 'G'.  It takes a fasta file as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void initializeString (string *newString);
void printSequence (string *sequence, FILE *outFile);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Input_filename Minimum_G_string_size\n", argv[0]);
        exit (1);
    }
//Main variables
    int minLength = atoi (argv[2]), count = 1, gCount = 0;
    char in;
    string sequence;
    FILE *inFile = NULL, *outFile = NULL;
//minLength check
    if (minLength == 0) {
        printf ("Minimum_length must be a numerical value greater than 0.\n");
        exit (3);
    }
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Correct the Gs
    printf ("Files opened.  Correcting entries...\n");
    initializeString (&sequence);
//Get to the first entry
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
//Load it manually
    while (in != '\n') {
        fprintf (outFile, "%c", in);
        in = fgetc (inFile);
    }
    fprintf (outFile, "\n");
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//If it's a new entry print the sequence then the next title
        if (in == '>') {
            printSequence (&sequence, outFile);
            reinitializeString (&sequence);
            fprintf (outFile, "\n");
            while (in != '\n') {
                fprintf (outFile, "%c", in);
                in = fgetc (inFile);
            }
            fprintf (outFile, "\n");
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0) {
                printf ("%d entries processed...\n", count);
            }
//Load the sequence
        } else if (in != '\n') {
//If we find 'G' look to see if it's long enough to be a break
            if (in == 'G') {
                while ((in == 'G') || (in =='\n')) {
                    if (in == 'G') {
                        gCount++;
                    }
//Stop if it's at the end
                    in = fgetc (inFile);
                    if (((ferror (inFile)) || (feof (inFile)))) {
                        break;
                    }
                }
//If it's big enough, replace with 'N'
                if (gCount >= minLength) {
                    while (gCount > 0) {
                        readValueToString (&sequence, 'N');
                        gCount--;
                    }
//If not, keep it 'G'
                } else {
                    while (gCount > 0) {
                        readValueToString (&sequence, 'G');
                        gCount--;
                    }
                }
//Rewind to have the loop deal with the next character
                fseek (inFile, -1, SEEK_CUR);
//If it's not a 'G' jsut add it
            } else {
                readValueToString (&sequence, in);
            }
        }
    }
//Print the last entry loaded
    printSequence (&sequence, outFile);
//Close everything and free memory
    printf ("%d entries processed.  Closing files and freeing memory...\n", count);
    free (sequence.str);
    fclose (inFile);
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
    outTitle = malloc (j + 13);
    outTitle[0] = '\0';
    strcat (outTitle, "G-Corrected_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Structured output function for fasta entries
void printSequence (string *sequence, FILE *outFile) {
//Local variables
    int curLoc = 0;
//Sequence has a 80 character/line limit
    while (curLoc < (sequence->len - 1)) {
        if ((curLoc != 0) && (curLoc % 80 == 0)) {
            fprintf (outFile, "\n");
        }
//One last quality control else/if chain before printing to file, this is to make sure only valid characters are printed.
        if (sequence->str[curLoc] == 'A') {
            fprintf (outFile, "A");
        } else if (sequence->str[curLoc] == 'C') {
            fprintf (outFile, "C");
        } else if (sequence->str[curLoc] == 'G') {
            fprintf (outFile, "G");
        } else if (sequence->str[curLoc] == 'T') {
            fprintf (outFile, "T");
        } else {
            fprintf (outFile, "N");
        }
        curLoc++;
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

//Resets a used string to an empty status
void reinitializeString (string *string) {
    free (string->str);
    initializeString (&(*string));
    return;
}
