/* This program updates a scaffold gff3 file so the locations match the new pseudochromosome positions.  It takes a gff3 file and a pseudochromosomes_scaffold_locations.txt file as inputs.*/

//Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Storage for the scaffold data
typedef struct scaffoldEntry {
    string oldTitle;
    string newTitle;
    int start;
    int length;
    char inverted;
    struct scaffoldEntry *next;
} scaffoldEntry;

//List of functions, alphabetically
void adjustGffEntries (scaffoldEntry *firScaff, FILE *inFile, FILE *outFile);
void buildScaffoldList (scaffoldEntry *firScaff, FILE *inFile);
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void createScaffoldEntry (scaffoldEntry **newEntry);
void freeScaffoldEntry (scaffoldEntry **entry);
void freeScaffoldList (scaffoldEntry *firScaff);
void initializeString (string *newString);
void readValueToString (string *string, char in);

//main ()
int main (int argC, char *argV[]) {
//Call syntax check
    if (argC != 3) {
        printf ("Usage: %s GFF_filename Pseudochromosomes_scaffold_locations_filename\n", argV[0]);
        exit (1);
    }
//Main variables
    FILE *gffInFile = NULL, *locationsInFile = NULL, *outFile = NULL;
    scaffoldEntry *firScaff = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&gffInFile, argV[1], 'r');
    createFile (&locationsInFile, argV[2], 'r');
    createOutputFile (&outFile, argV[1]);
    createScaffoldEntry (&firScaff);
//Parse the titles and starts from the locations file
    printf ("Files found and created.  Parsing scaffold titles...\n");
    buildScaffoldList (firScaff, locationsInFile);
    fclose (locationsInFile);
//Adjust the gff file entries
    printf ("Adjusting Gff entries...\n");
    adjustGffEntries (firScaff, gffInFile, outFile);
//Close everything
    printf ("Freeing memory and closing files...\n");
    freeScaffoldList (firScaff);
    fclose (gffInFile);
    fclose (outFile);
    printf ("Done.\n");
    return 0;
}

//Update the start and stop positions of the entries in a gff file
void adjustGffEntries (scaffoldEntry *firScaff, FILE *inFile, FILE *outFile) {
//Local variables
    string scaffold, score;
    scaffoldEntry *curScaff = NULL;
    char in, strand;
    int start, end, tempStart, count = 0;
//Automate the process
    while (1) {
//Start the new line, reset the scaffold pointer and scaffold string
        in = fgetc (inFile);
        curScaff = firScaff;
        initializeString (&scaffold);
        initializeString (&score);
//Break conditions
        if ((feof (inFile)) || (ferror (inFile))) {
            break;
//If it is a spacer/comment line
        } else if (in == '#') {
//Just copy the entire line
            while (in != '\n') {
                fprintf (outFile, "%c", in);
                in = fgetc (inFile);
            }
            fprintf (outFile, "\n");
//If it is an entry line
        } else {
//Read the old scaffold title
            while (in != '\t') {
                readValueToString (&scaffold, in);
                in = fgetc (inFile);
            }
//Find the corresponding pseudochromosome title entry, if there is one
            while ((strcmp (scaffold.str, curScaff->oldTitle.str) != 0) && (curScaff->next != NULL)) {
                curScaff = curScaff->next;
            }
//If there is no matching entry just copy the entire line
            if (curScaff->next == NULL) {
                fprintf (outFile, "%s", scaffold.str);
                while (in != '\n') {
                    fprintf (outFile, "%c", in);
                    in = fgetc (inFile);
                }
                fprintf (outFile, "\n");
//Otherwise
            } else {
//Print the new title
                fprintf (outFile, "%s%c", curScaff->newTitle.str, in);
//Copy up to the next \t
                in = fgetc (inFile);
                while (in != '\t') {
                    fprintf (outFile, "%c", in);
                    in = fgetc (inFile);
                }
                fprintf (outFile, "\t");
//Copy up to the next \t, again
                in = fgetc (inFile);
                while (in != '\t') {
                    fprintf (outFile, "%c", in);
                    in = fgetc (inFile);
                }
                fprintf (outFile, "\t");
//Read the start and end from the gff
                fscanf (inFile, "%d%*c%d%*c", &start, &end);
//Read whatever value is stored as a score from the gff
                in = fgetc (inFile);
                while (in != '\t') {
                    readValueToString (&score, in);
                    in = fgetc (inFile);
                }
//Read the strand from the gff
                strand = fgetc (inFile);
//If the scaffold was inverted
                if (curScaff->inverted == '-') {
//Switch the strand
                    if (strand == '-') {
                        strand = '+';
                    } else {
                        strand = '-';
                    }
//Calculate the new start and end coordinates
                    tempStart = curScaff->length - end;
                    end = curScaff->length - start;
                    start = tempStart;
                }
//Adjust the start and end points to match the scaffold's new position in the pseudochromosome
                start += (curScaff->start - 1);
                end += (curScaff->start - 1);
//Print the data
                fprintf (outFile, "%d\t%d\t%s\t%c", start, end, score.str, strand);
//Copy the rest of the line
                in = fgetc (inFile);
                while (in != '\n') {
                    fprintf (outFile, "%c", in);
                    in = fgetc (inFile);
                }
                fprintf (outFile, "\n");
//A counter so the user has some idea of how long it will take
                if (++count % 100000 == 0){
                    printf ("%d gff entries adjsuted...\n", count);
                }
            }
        }
    }
    printf ("%d gff entries adjsuted...\n", count);
    return;
}

//Collect title and start data from a Pseudochromosomes_scaffold_locations.txt file
void buildScaffoldList (scaffoldEntry *firScaff, FILE *inFile) {
//Local variables
    scaffoldEntry *curScaff = firScaff, *prevScaff;
    char in;
    int count = 0;
//Automate the process
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if ((feof (inFile)) || (ferror (inFile))) {
            break;
        } else {
//Read the scaffold title
            while (in != '\t') {
                readValueToString (&curScaff->oldTitle, in);
                in = fgetc (inFile);
            }
//Read the pseudochromosome title
            in = fgetc (inFile);
            while (in != ':') {
                readValueToString (&curScaff->newTitle, in);
                in = fgetc (inFile);
            }
//Read the start position and the inverted value
            fscanf (inFile, "%d%*c%d%*c%c%*c", &curScaff->start, &curScaff->length, &curScaff->inverted);
//Create and move to the next empty node
            prevScaff = curScaff;
            createScaffoldEntry (&curScaff->next);
            curScaff = curScaff->next;
//A counter so the user has some idea of how long it will take
            if (++count % 1000 == 0){
                printf ("%d scaffold titles parsed...\n", count);
            }
        }
    }
//Delete the empty last node
    prevScaff->next = NULL;
    freeScaffoldEntry (&curScaff);
    printf ("%d scaffold titles parsed.  ", count);
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
    while (i < strlen (inName)) {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 27);
    outTitle[0] = '\0';
    strcat (outTitle, "Pseudochromosome_adjusted_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Create a new node on a scaffoldEntry list
void createScaffoldEntry (scaffoldEntry **newEntry) {
    *newEntry = malloc (sizeof (**newEntry));
    initializeString (&(*newEntry)->oldTitle);
    initializeString (&(*newEntry)->newTitle);
    (*newEntry)->next = NULL;
    return;
}

//Frees all data alloc's by createScaffoldEntry()
void freeScaffoldEntry (scaffoldEntry **entry) {
    free ((*entry)->oldTitle.str);
    free ((*entry)->newTitle.str);
    free (*entry);
    return;
}

//Frees a list of scaffoldEntry's
void freeScaffoldList (scaffoldEntry *firScaff){
    scaffoldEntry *curScaff = firScaff->next;
    while (firScaff->next != NULL) {
        freeScaffoldEntry (&firScaff);
        firScaff = curScaff;
        curScaff = curScaff->next;
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
