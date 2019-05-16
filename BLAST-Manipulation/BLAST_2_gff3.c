/* This program Parses out a default tab delimited BLAST output.  It takes a tabular BLAST output file as input */

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
void generateGff (FILE *inFile, FILE *outFile);
void initializeString (string *newString);
void readValueToString (string *string, char in);

//main()
int main (int argc, char *argv[]) {
//Call syntax checks
    if (argc != 2) {
        printf ("Usage: %s BLAST_output_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1], NULL, ".gff");
//Parse out the data
    generateGff (inFile, outFile);
//Close everything
    printf ("  Closing files and freeing memory...\n");
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

//Parse the data into a gff format
void generateGff (FILE *inFile, FILE *outFile) {
//Local variables
    string name, target, eValue;
    int count = 0, start, end, temp;
    char in, strand;
//Header
    fprintf (outFile, "##gff-version   3\n");
//Loop it all
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
        initializeString (&name);
        initializeString (&target);
        initializeString (&eValue);
//Read the name
        while (in != '\t') {
            readValueToString (&name, in);
            in = getc (inFile);
        }
        in = fgetc (inFile);
//Read the entry
        while (in != '\t') {
            readValueToString (&target, in);
            in = getc (inFile);
        }
//Collect the #s
        fscanf (inFile, "%*f%*c%*d%*c%*d%*c%*d%*c%*d%*c%*d%*c%d%*c%d", &start, &end);
//Collect evalue
        in = fgetc (inFile);
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&eValue, in);
            in = getc (inFile);
        }
//Burn the rest of the line
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Determine strand
        if (start < end) {
            strand = '+';
        } else {
            strand = '-';
            temp = start;
            start = end;
            end = temp;
        }
//Print the result
        fprintf (outFile, "%s\tBLAST\tncRNA\t%d\t%d\t%s\t%c\t.\tName=%s\n", target.str, start, end, eValue.str, strand, name.str);
//Free the strings variables
        free (name.str);
        free (target.str);
//A counter so the user has some idea of time frame.
        if (++count % 10000 == 0) {
            printf ("Entry %d compared...\n", count);
        }
    }
    printf ("%d entries compared.", count);
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
