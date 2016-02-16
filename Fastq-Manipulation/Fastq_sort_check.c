/* This program checks that the entries in two fastqs are in the same order, listing any that do not match.  It takes the shared part of the R1 and R2 files as input. */

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
void createFile (FILE **file, char *fname, char perm);
void createOutputFile (FILE **outFile, char *inName);
void initializeString (string *newString);
void loadFastqTitle (string *title, FILE *source);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Shared_input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    int count = 0, orphanCount = 0;
    FILE *forwardInFile = NULL, *reverseInFile = NULL, *outFile = NULL;
    string forwardTitle, reverseTitle;
//File creation and checks
    printf ("Opening files..\n");
    initializeString (&forwardTitle);
    initializeString (&reverseTitle);
    forwardTitle.len = strlen (argv[1]) + 10;
    forwardTitle.str = realloc (forwardTitle.str, forwardTitle.len);
    strcat (forwardTitle.str, argv[1]);
    strcat (forwardTitle.str, "_R1.fastq");
    createFile (&forwardInFile, forwardTitle.str, 'r');
    forwardTitle.str[(forwardTitle.len - 8)] = '2';
    createFile (&reverseInFile, forwardTitle.str, 'r');
    createOutputFile (&outFile, argv[1]);
    reinitializeString (&forwardTitle);
//Compare the entries
    printf ("Files opened.  Comparing entries...\n");
    while (1) {
//Load the forward and reverse data
        loadFastqTitle (&forwardTitle, forwardInFile);
        loadFastqTitle (&reverseTitle, reverseInFile);
//If one list is done, stop the loop
        if ((forwardTitle.str == NULL) || (reverseTitle.str == NULL)) {
            break;
        }
//If they don't match, report it
        if (strcmp (forwardTitle.str, reverseTitle.str) != 0) {
            fprintf (outFile, "%s\t%s\n", forwardTitle.str, reverseTitle.str);
        }
//A counter so the user has some idea of how long it will take
        if (++count % 1000000 == 0) {
            printf ("%d entries compared...\n", count);
        }
//Reset the entries
        reinitializeString (&forwardTitle);
        reinitializeString (&reverseTitle);
    }
//Check if one list is longer than the other
    printf ("%d entries compared.  Checking for orphan entries...\n", count);
//If the reverse list was shorter
    while (forwardTitle.str != NULL) {
//Report the orphan
        fprintf (outFile, "%s\t\n", forwardTitle.str);
        orphanCount++;
//Clear the current data and load the next entry
        reinitializeString (&forwardTitle);
        loadFastqTitle (&forwardTitle, forwardInFile);
    }
//If the forward list was shorter
    while (reverseTitle.str != NULL) {
//Report the orphan
        fprintf (outFile, "\t%s\n", reverseTitle.str);
        orphanCount++;
//Clear the current data and load the next entry
        reinitializeString (&reverseTitle);
        loadFastqTitle (&reverseTitle, reverseInFile);
    }
//Close everything and free memory
    printf ("%d orphaned entries found.  Closing files and freeing memory...\n", orphanCount);
    fclose (forwardInFile);
    fclose (reverseInFile);
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
    outTitle = malloc (j + 16);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_mismatches.txt");
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

//Finds the title of the next entry in a file, sets the string to NULL at EOF
void loadFastqTitle (string *title, FILE *source) {
//Local variables
    char in;
//Find the first entry
    in = fgetc (source);
    while (in != '\n') {
//Set the string to NULL if EOF
        if (((ferror (source)) || (feof (source)))) {
            free (title->str);
            title->str = NULL;
            return;
        }
//Otherwise add it to the title
        readValueToString (title, in);
        in = fgetc (source);
    }
//Get the '+'
    while (in != '+') {
        in = fgetc (source);
    }
    fgetc (source);
//Skip the quality
    in = fgetc (source);
    while (in != '\n') {
        if (((ferror (source)) || (feof (source)))) {
            break;
        }
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
