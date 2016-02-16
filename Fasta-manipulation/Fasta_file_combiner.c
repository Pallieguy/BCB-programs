/* This program combines a list titles and a list of sequences into a fasta.  It takes a file of titles and a file of single line sequences as input. */

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
void loadString (string *string, FILE *source);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Title_list_filename Sequence_list_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    string title, sequence;
    FILE *titleInFile = NULL, *sequenceInFile = NULL, *outFile = NULL;
    int count = 0;
//File creation and checks
    printf ("Opening files..\n");
    createFile (&titleInFile, argv[1], 'r');
    createFile (&sequenceInFile, argv[2], 'r');
    createFile (&outFile, "Combined.fasta", 'w');
//Combine entries
    printf ("File opened.  Combining...\n");
    initializeString (&title);
    initializeString (&sequence);
    while (1) {
//Load the data, break the loop if either list runs out
        loadString (&title, titleInFile);
        loadString (&sequence, sequenceInFile);
        if ((title.str == NULL) || (sequence.str == NULL)){
            break;
        }
//Print the new fasta entry and reset for the next set
        fprintf (outFile, ">%s\n%s\n", title.str, sequence.str);
        reinitializeString (&title);
        reinitializeString (&sequence);
//A counter so the user has some idea of how long it will take
        if (++count % 1000000 == 0) {
            printf ("%d entries combined...\n", count);
        }
    }
//Close everything and free memory
    printf ("%d entries combined.  Closing files and freeing memory...\n", count);
    free (title.str);
    free (sequence.str);
    fclose (titleInFile);
    fclose (sequenceInFile);
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

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Read a line from a file into a string
void loadString (string *string, FILE *source) {
    char in;
    in = fgetc (source);
    while (in != '\n') {
        if (((ferror (source)) || (feof (source)))) {
            string->str = NULL;
            break;
        }
        readValueToString (string, in);
        in = fgetc (source);
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
