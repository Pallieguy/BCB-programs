/* This program counts the number of XLOCs holding a non-zero fpkm from a cummeRbund fpkm generated csv.  It takes a csv as input. */

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
void copyString (string *destination, string *source);
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
    string curSample, sample;
    char in;
    int count = 0;
    float fpkm = 0;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
//Collect stats
    printf ("Files opened.  Parsing stats...\n");
    initializeString (&curSample);
    in = fgetc (inFile);
//Skip the first line
    while (in != '\n') {
        in = fgetc (inFile);
    }
//Automate the rest
    while (1) {
//prep sample to be loadable
        initializeString (&sample);
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Skip to the sample_name
        while (in != ',') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != ',') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Read sample_name
        while (in != ',') {
            readValueToString (&sample, in);
            in = fgetc (inFile);
        }
//If it's the first sample, load curSample
        if (curSample.len == 1) {
            copyString (&curSample, &sample);
        }
//Read the fpkm value and adjust as needed if fpkm isn't 0
        fscanf (inFile, "%f", &fpkm);
        if (fpkm != 0) {
//If it's from the same sample increase the count
            if (strcmp (curSample.str, sample.str) == 0) {
                count++;
//If it's from a different sample
            } else {
                printf ("%s\t%d genes\n", curSample.str, count);
                count = 1;
                copyString (&curSample, &sample);
            }
        }
//Skip to the next line and empty sample so it can be used again
        while (in != '\n') {
            in = fgetc (inFile);
        }
        free (sample.str);
        sample.len = 0;
    }
//Print the last sample_name and count
    printf ("%s\t%d genes\n", curSample.str, count);
//Close everything and free memory
    free (sample.str);
    free (curSample.str);
    fclose (inFile);
    return 0;
}

//Copy the data to one string from another
void copyString (string *destination, string *source) {
//Local variables
    int i;
    destination->len = 1;
    for (i = 0; i != source->len; i++) {
        readValueToString (destination, source->str[i]);
    }
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
