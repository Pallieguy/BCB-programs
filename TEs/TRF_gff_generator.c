/* This program generates a gff3 of TRF data.  It takes a TRF output file as input. May need some edits when used next*/
/*REBUILD THIS MAYBE? IT'S A BAND-AID UPDATE FORM WEIRD OLD CODE*/

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
void readValueToString (string *string, char in);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s TRF_dat_output_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    char in;
    int start, end, size, count = 0, tempend, score;
    string title;
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Skip the header
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
    in = fgetc (inFile);
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
    initializeString (&title);
//Parse out entry data
    printf ("Generating entries...\n");
    fprintf (outFile, "##gff-version   3\n");
//Automate the rest
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Skip blank lines
        if (in != '\n') {

//If it's a new sequence load the title
            if (in == 'S') {
                while (in != ' ') {
                    in = fgetc (inFile);
                }
                if (title.len != 1) {
                    free (title.str);
                    title.len = 0;
                }
                initializeString (&title);
                in = fgetc (inFile);
                while (in != '\n') {
                    readValueToString (&title, in);
                    in = fgetc (inFile);
                }
//Burn to the entry info
                in = fgetc (inFile);
                in = fgetc (inFile);
                in = fgetc (inFile);
                in = fgetc (inFile);
                while (in != '\n') {
                    in = fgetc (inFile);
                }
                in = fgetc (inFile);
                in = fgetc (inFile);
//If it's not a new entry rewind 1 position
            } else {
                fseek (inFile, -1, SEEK_CUR);
            }
//Otherwise copy the data
            fscanf (inFile, "%d%d%d%*s%*s%*s%*s%d", &start, &end, &size, &score);
//Print the entries
            fprintf (outFile, "%s\tTRF\trepeat_region\t%d\t%d\t%d\t?\t.\tID=TRF_repeat%d\n", title.str, start, end, score, ++count);
//Break down the elements
            while (start < end) {
                if ((start + size) < end) {
                    tempend = (start + size);
                } else {
                    tempend = end;
                }
                fprintf (outFile, "%s\tTRF\ttandem_repeat\t%d\t%d\t.\t?\t.\tParent=TRF_repeat%d\n", title.str, start, tempend, count);
                start = ++tempend;
            }
            fprintf (outFile, "###\n");
//A counter so the user has some idea of how long it will take
            if (count % 10000 == 0) {
                printf ("%d entries generated...\n", count);
            }
//Burn to the end of the line
            while (in != '\n') {
                in = fgetc (inFile);
//Stop conditions
                if (((ferror (inFile)) || (feof (inFile)))) {
                    break;
                }
            }
        }
    }
//Close everything and free memory
    printf ("%d entries generated.  Closing files and freeing memory...\n", count);
    fclose (inFile);
    fclose (outFile);
    printf ("Done.\n");
    return(0);
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
    outTitle = malloc (j + 6);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, ".gff3");
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
