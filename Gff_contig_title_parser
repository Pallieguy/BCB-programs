/* This program generates a list of contig titles and sizes from gff3 entries.  It takes a contig.gff3 file as input. */

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
void createOutputFile (FILE **outFile, char *inName);
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
//Main varaibles
    char in;
    string title, type;
    FILE *inFile = NULL, *outFile = NULL;
    int count = 0, sequenceLength = 0, netSequenceLength = 0;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Parse the titles
    printf ("Files opened.  Parsing titles...\n");
    initializeString (&title);
    initializeString (&type);
    in = fgetc (inFile);
//Skip the first line if it is a comment line
    if (in == '#') {
        while (in != '\n') {
            in = fgetc (inFile);
        }
    }
    in = fgetc (inFile);
    while (1) {
//Read the title
        while (in != '\t') {
            if (((ferror (inFile)) || (feof (inFile)))) {
                title.str = NULL;
                break;
            }
            readValueToString (&title, in);
            in = fgetc (inFile);
        }
//Stop the loop if the list is done
        if (title.str == NULL) {
            break;
        }
//Move the file pointer to the type
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
//If it's a contig read the end position then print the values
        if (strcmp (type.str, "contig") == 0) {
            fscanf (inFile, "%*d%*c%d", &sequenceLength);
            netSequenceLength += sequenceLength;
            fprintf (outFile, ">%s\t%d\t%d\n", title.str, sequenceLength, netSequenceLength);
        }
//Burn the rest of the line and reset the strings
        while (in != '\n') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        reinitializeString (&title);
        reinitializeString (&type);
//A counter so the user has some idea of how long it will take
        if (++count % 100000 == 0) {
            printf ("%d entries processed...\n", count);
        }
    }
//Close everything and free memory
    printf ("%d entries processed.  Closing files and freeing memory...\n", count);
    free (title.str);
    free (type.str);
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
//Remove the file extention.  EDIT THIS AT COPY
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
    outTitle = malloc (j + 19);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_contig_titles.txt");
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
