/* This program finds the three preceding and proceding amino acid single letters flanking a F from an input AA fasta sequence and prints them.  Missing values are replaced by 'X'.  It takes a generic text file as input. */

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
void findK (FILE *inFile, FILE *outFile);
void readValueToString (string *string, char in);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Build dynamic linked list of scaf data
    printf ("Files found and created.  Generating list...\n");
    findK (inFile, outFile);
//Close everything
    printf ("Closing files and freeing memory...\n");
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
    while (i < strlen (inName)) {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 8);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, "_out.txt");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Does pretty much everything
void findK (FILE *inFile, FILE *outFile) {
//Loacl variables
    string input;
    char in, check;
    int kPos, i;
//Skip the first 2 lines
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
//Loop the rest
    while (1) {
        in = fgetc (inFile);
        initializeString (&input);
        kPos = 0;
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        } 
//Load the string
        while (in != '\n') {
            readValueToString (&input, in);
            in = fgetc (inFile);
        }
//Find 'K'
        check = input.str[kPos];
        while (check != 'K') {
            if (check == '\0') {
                printf ("STRING:%s has no 'K'!\n", input.str);
                exit (1);
            }
            kPos++;
            check = input.str[kPos];
        }
//Check for enough preceding characters
        if (kPos == 0) {
            fprintf (outFile, "XXX");
        } else if (kPos == 1) {
            fprintf (outFile, "XX%c", input.str[(kPos - 1)]);
        } else if (kPos == 2) {
            fprintf (outFile, "X%c%c", input.str[(kPos - 2)], input.str[(kPos - 1)]);
        } else {
            fprintf (outFile, "%c%c%c", input.str[(kPos - 3)], input.str[(kPos - 2)], input.str[(kPos - 1)]);
//print the K
        }
            fprintf (outFile, "K");
//Check if there are enough proceding characters
        if ((kPos + 2) == input.len) {
            fprintf (outFile, "XXX");
        } else if ((kPos + 3) == input.len) {
            fprintf (outFile, "%cXX", input.str[(kPos + 1)]);
        } else if ((kPos + 4) == input.len) {
            fprintf (outFile, "%c%cX", input.str[(kPos + 1)], input.str[(kPos + 2)]);
        } else {
            fprintf (outFile, "%c%c%c", input.str[(kPos + 1)], input.str[(kPos + 2)], input.str[(kPos + 3)]);
        }
//Newline for the next entry
        fprintf (outFile, "\n");
//Clean the string
        free (input.str);
        input.len = 0;
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
    string->str[((string->len) - 2)] = in;
    string->str[((string->len) - 1)] = '\0';
    return;
}
