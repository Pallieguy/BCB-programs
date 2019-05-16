/* This program generates a gff3 of a PoI csv.  It takes a csv file as input. */

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
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix);
void generatePoI (FILE *inFile, FILE *p1OutFile, FILE *p2OutFile, char *p1Name, char *p2Name);
void initializeString (string *newString);
void readValueToString (string *string, char in);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 4) {
        printf ("Usage: %s CSV_filename Parent_1_name Parent_2_name\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *p1OutFile = NULL, *p2OutFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&p1OutFile, argv[1], argv[2], ".gff3");
    createOutputFile (&p2OutFile, argv[1], argv[3], ".gff3");
//Parse out entry data
    printf ("Files opened.  Generating entries...\n");
    generatePoI (inFile, p1OutFile, p2OutFile, argv[2], argv[3]);
//Close everything and free memory
    printf ("  Closing files and freeing memory...\n");
    fclose (inFile);
    fclose (p1OutFile);
    fclose (p2OutFile);
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
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix) {
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
//Remove the file extention.
    while (j-- > 0) {
        if (inName[j] == '.') {
            inName[j] = '\0';
            break;
        }
    }
    j = 0;
//Pull the filename from the input argument
    fileName = malloc (strlen (inName) - i + 1);
    fileName[0] = '\0';
    while (inName[i] != '\0') {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Adjsut title length
    if (prefix != NULL && suffix == NULL) {
        outTitle = malloc (j + strlen (prefix) + 2);
    } else if (prefix == NULL && suffix != NULL) {
        outTitle = malloc (j + strlen (suffix) + 1);
    } else {
        outTitle = malloc (j + strlen (prefix) + strlen (suffix) + 2);
    }
//Build the filename
    outTitle[0] = '\0';
    if (prefix != NULL) {
        strcat (outTitle, prefix);
        strcat (outTitle, "_");
    }
    strcat (outTitle, fileName);
    if (suffix != NULL) {
        strcat (outTitle, suffix);
    }
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Generates the gffs for each parentage
void generatePoI (FILE *inFile, FILE *p1OutFile, FILE *p2OutFile, char *p1Name, char *p2Name) {
//Local variables
    int count = 0, p1Location = 0, p1Start = 0, p2Location = 0, p2Start = 0, matchStart = 0;
    char in;
    string p1Title, p2Title, matchTitle;
    float ratio;
//prime each output
    fprintf (p1OutFile, "##gff-version 3\n");
    fprintf (p2OutFile, "##gff-version 3\n");
//Skip the header lines
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
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
        initializeString (&p1Title);
        initializeString (&p2Title);
        initializeString (&matchTitle);
//If there is no entry for parent 1
        if (in == ',') {
//Skip to parent 2
            while (in == ',') {
                in = fgetc (inFile);
            }
            fscanf (inFile, "%*d%*c%*d%*c");
            in = fgetc (inFile);
            p1Location = 0;
//Otherwise load the parent 1 values 
        } else {
            while (in != ',') {
                in = fgetc (inFile);
            }
            fscanf (inFile, "%d%*c%d%*c%*d%*c%*d%*c%*d%*c%*d%*c", &p1Location, &p1Start);
            in = fgetc (inFile);
        }
//If there is no entry for parent 2
        if (in == ',') {
//Skip to match values
            while (in == ',') {
                in = fgetc (inFile);
            }
            p2Location = 0;
//Otherwise load parent 2 values
        } else {
            while (in != ',') {
                readValueToString (&p2Title, in);
                in = fgetc (inFile);
            }
            fscanf (inFile, "%d%*c%*d%*c%d%*c", &p2Start, &p2Location);
//Skip to match values
            in = fgetc (inFile);
            while (in != ',') {
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
        }
//Load match values and ratio
        while (in != ',') {
            readValueToString (&matchTitle, in);
            in = fgetc (inFile);
        }
        fscanf (inFile, "%d%*c%*c%f", &matchStart, &ratio);
//Burn the rest of the line
        while (in != '\n') {
            in = fgetc (inFile);
//Stop conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Sanity check on the formatting of the csv
        if ((p1Location != 0) && (p2Location != 0)) {
            if (p1Location != p2Location) {
                printf ("Entry %d (%s) has mismatch between %d and %d.\n", count, matchTitle.str, p1Location, p2Location);
                exit (2);
            }
        }
//If it's a parent 1 entry
        if (ratio > 1) {
            fprintf (p1OutFile, "%s\tPoI_pipeline\tnucleotide_match\t%d\t%d\t%f\t.\t.\tName=%s-%s:%d_match\n", matchTitle.str, p1Location, (p1Location + 99), ratio, p1Name, p1Title.str, p1Start);
//Otherwise if it's a parent 2 entry
        } else if (ratio < 1) {
            fprintf (p2OutFile, "%s\tPoI_pipeline\tnucleotide_match\t%d\t%d\t%f\t.\t.\tName=%s-%s:%d_match\n", matchTitle.str, p2Location, (p2Location + 99), ratio, p2Name, p2Title.str, p2Start);
//Or if it's a shared entry
        } else {
            fprintf (p1OutFile, "%s\tPoI_pipeline\tnucleotide_match\t%d\t%d\t%f\t.\t.\tName=%s-%s:%d_match\n", matchTitle.str, p1Location, (p1Location + 99), ratio, p1Name, p1Title.str, p1Start);
            fprintf (p2OutFile, "%s\tPoI_pipeline\tnucleotide_match\t%d\t%d\t%f\t.\t.\tName=%s-%s:%d_match\n", matchTitle.str, p2Location, (p2Location + 99), ratio, p2Name, p2Title.str, p2Start);
        }
//Clean up before the next entry
        free (p1Title.str);
        p1Title.len = 0;
        free (p2Title.str);
        p2Title.len = 0;
        free (matchTitle.str);
        matchTitle.len = 0;        
//A counter so the user has some idea of how long it will take
        if (++count % 100 == 0) {
            printf ("%d entries parsed...\n", count);
        }
    }
    printf ("%d entries parsed.", count);
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
