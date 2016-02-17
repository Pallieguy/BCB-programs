/* This program generates a gff3 of LTR_Finder data.  It takes a LTR_Finder output file as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Easy gff data storage
typedef struct entry {
    string title;
    char strand;
    int score;
    int start5LTR;
    int end5LTR;
    int start3LTR;
    int end3LTR;
    int start5TSR;
    int end5TSR;
    int start3TSR;
    int end3TSR;
} entry;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void initializeEntry (entry **newEntry);
void initializeString (string *newString);
void readValueToString (string *string, char in);
void reinitializeString (string *oldString);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s LTR_Finder_ouput_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    char in;
    int count = 1;
    entry *entry;
    initializeEntry (&entry);
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Parse out entry data
    printf ("Generating entries...\n");
    fprintf (outFile, "##gff-version 3\n");
    in = fgetc (inFile);
    while (1) {
        in = fgetc (inFile);
//Stop conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//An Entry will be triggered by a [
        if (in == '[') {
//Skip to the read title
            while (in != ' ') {
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
//Load the read title
            while (in != ' ') {
                readValueToString (&entry->title, in);
                in = fgetc (inFile);
            }
//Skip to the strand
            while (in != 'S') {
                in = fgetc (inFile);
            }
            while (in != ':') {
                in = fgetc (inFile);
            }
//Read strand
            in = fgetc (inFile);
            entry->strand = in;
//Skip to score
            while (in != '.') {
                in = fgetc (inFile);
            }
//Read score
            fscanf (inFile, "%d", &entry->score);
//Skip to 5'-LTR start
            while (in != 'R') {
                in = fgetc (inFile);
            }
            while (in != ':') {
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
//Read 5'LTR start and end
            fscanf (inFile, "%d%*c%*c%d", &entry->start5LTR, &entry->end5LTR);
//Repeat for 3'LTR
            while (in != 'R') {
                in = fgetc (inFile);
            }
            while (in != ':') {
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
            fscanf (inFile, "%d%*c%*c%d", &entry->start3LTR, &entry->end3LTR);
//Skip to TSR
            while (in != 'R') {
                in = fgetc (inFile);
            }
            while (in != ':') {
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
//Read TSRs
            fscanf (inFile, "%d%*c%*c%d%*c%*c%d%*c%*c%d", &entry->start5TSR, &entry->end5TSR, &entry->start3TSR, &entry->end3TSR);
//Print the data
            fprintf (outFile, "%s\tLTR_Finder\trepeat_region\t%d\t%d\t.\t%c\t.\tID=repeat_region%d\n%s\tLTR_Finder\ttarget_site_duplication\t%d\t%d\t.\t%c\t.\tParent=repeat_region%d\n%s\tLTR_Finder\tLTR_retrotransposon\t%d\t%d\t.\t%c\t.\tID=LTR_retrotransposon%d;Parent=repeat_region%d;LTR_similarity=0.%d\n%s\tLTR_Finder\tlong_terminal_repeat\t%d\t%d\t.\t%c\t.\tParent=repeat_region%d\n%s\tLTR_Finder\tlong_terminal_repeat\t%d\t%d\t.\t%c\t.\tParent=repeat_region%d\n%s\tLTR_Finder\ttarget_site_duplication\t%d\t%d\t.\t%c\t.\tParent=repeat_region%d\n###\n", entry->title.str, entry->start5TSR, entry->end3TSR, entry->strand, count, entry->title.str, entry->start5TSR, entry->end5TSR, entry->strand, count, entry->title.str, entry->start5LTR, entry->end3LTR, entry->strand, count, count, entry->score, entry->title.str, entry->start5LTR, entry->end5LTR, entry->strand, count, entry->title.str, entry->start3LTR, entry->end3LTR, entry->strand, count, entry->title.str, entry->start3TSR, entry->end3TSR, entry->strand, count);
//Reset the entry
            reinitializeString (&entry->title);
//A counter so the user has some idea of how long it will take
            if (count++ % 100 == 0) {
                printf ("%d entries parsed...\n", count - 1);
            }
//Move past the exact match to start looking for the next [
            while (in != 'L') {
                in = fgetc (inFile);
            }
        }
    }
//Close everything and free memory
    printf ("%d Entries generated.  Closing files and freeing memory...\n", count);
    free (entry->title.str);
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

//Sets minimum values to an entry
void initializeEntry (entry **newEntry) {
    *newEntry = malloc (sizeof (**newEntry));
    initializeString (&(*newEntry)->title);
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

//Resets a string to minimum values
void reinitializeString (string *oldString) {
    free (oldString->str);
    initializeString (oldString);
    return;
}
