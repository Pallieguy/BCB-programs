/* This program generates the number of entries, their total size, and the average size/entry from a fastq.  It takes a fastq as input. */

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
void initializeString (string *newString);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    int sequenceLength = 0, count = 0;
    long netSequenceLength = 0;
    FILE *inFile = NULL, *outFile = NULL;
    string entry;
    char in;
//File creation and checks
    createFile (&inFile, argv[1], 'r');
//Extract titles
    initializeString (&entry);
//Prime the search and loop it
    while (1) {
    in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Skip the titles
        if (in == '@') {
            while (in != '\n') {
                in = fgetc (inFile);
            }
        }
/*If you need to abstract out to more than 1 line per entry edit here (also figure out how)*/
//Count the sequence length
        in = fgetc (inFile);
        while (in != '\n') {
            readValueToString (&entry, in);
            in = fgetc (inFile);
        }
//Skip the +
        in = fgetc (inFile);
        in = fgetc (inFile);
        in = fgetc (inFile);
/*If you need to abstract out to more than 1 line per entry edit here (also figure out how)*/
//Skip the quality
        while (in != '\n') {
//Break at EOF
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            in = fgetc (inFile);
        }
//Adjsut the length
        netSequenceLength += entry.len - 1;
        reinitializeString (&entry);
//A counter so the user has some idea of how long it will take
        count++;
    }
//Display stats and free memory
    printf ("%s contains: %d entries, %ld total nucleotides, average length of %ld nucleotides.\n", argv[1], count, 
netSequenceLength, (netSequenceLength / count));
    free (entry.str);
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
