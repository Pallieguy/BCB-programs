/* This program interleaves two title into a single file.  It takes two title lists as input.  I suspect this is a one off program for the Chr08 interleaving.*/

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Standard data from a fasta entry
typedef struct titleEntry {
    string scaffold;
    int start;
    int end;
    int size;
    char strand;
    struct titleEntry *next;
} titleEntry;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);
void freeTitleEntry (titleEntry *oldEntry);
void initializeTitleEntry (titleEntry *newEntry);
void initializeString (string *newString);
void interleaveTitles (titleEntry *firEntry1, titleEntry *firEntry2, FILE *outFile);
void loadTitleEntryList (titleEntry *firEntry, FILE *inFile);
void printTitleEntry (titleEntry *entry, FILE *outFile);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s First_title_filename Second_title_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    int count = 0;
    FILE *inFile1 = NULL, *inFile2 = NULL, *outFile = NULL;
    titleEntry firEntry1, firEntry2;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile1, argv[1], 'r');
    createFile (&inFile2, argv[2], 'r');
    createOutputFile (&outFile, "OAC-Rex_titles.txt");
//Load the entries from the first list
    printf ("Files opened.  Loading first titles list...\n");
    initializeTitleEntry (&firEntry1);
    loadTitleEntryList (&firEntry1, inFile1);
    fclose (inFile1);
//Load the entries from the second list
    printf ("Loading second titles list...\n");
    initializeTitleEntry (&firEntry2);
    loadTitleEntryList (&firEntry2, inFile2);
    fclose (inFile2);
//Interleave the titles in ascending order by their start positions
    printf ("Interleaving titles...\n");
    interleaveTitles (&firEntry1, &firEntry2, outFile);
//Close everything
    printf ("Closing files and freeing memory...\n");
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
    outTitle = malloc (j + 12);
    outTitle[0] = '\0';
    strcat (outTitle, "interleaved_");
    strcat (outTitle, fileName);
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}

//Frees the memory used by a fastaEntry node
void freeTitleEntry (titleEntry *oldEntry) {
    free (oldEntry->scaffold.str);
    free (oldEntry->next);
    return;
}

//Sets the minimum values to a fastaEntry
void initializeTitleEntry (titleEntry *newEntry) {
    initializeString (&newEntry->scaffold);
    newEntry->start = 0;
    newEntry->end = 0;
    newEntry->size = 0;
    newEntry->strand = ' ';
    newEntry->next = NULL;
    return;
}

//Sets minimum values to a string
void initializeString (string *newString) {
    newString->len = 1;
    newString->str = malloc (1);
    newString->str[0] = '\0';
    return;
}

//Interleaves the lists together in ascending order
void interleaveTitles (titleEntry *firEntry1, titleEntry *firEntry2, FILE *outFile) {
//Local variables
    titleEntry *curEntry = NULL;
    int lastPrint = 0;
//point curEntry at something to start
    curEntry = firEntry1;
//Loop the process
    while (1) {
//Break once both lists are exhausted
        if ((firEntry1 == NULL) && (firEntry2 == NULL)) {
            break;
        }
//If there's still entries in each list
        if ((firEntry1 != NULL) && (firEntry2 != NULL)) {
//If list1 is earlier
            if (firEntry1->start < firEntry2->start) {
                curEntry = firEntry1->next;
//Find the last entry before the first entry on list2
                while (curEntry->start < firEntry2->start) {
                    freeTitleEntry (firEntry1);
                    firEntry1 = curEntry;
                    curEntry = firEntry1->next;
               }
//Check that there isn't an impossible overlap
               if (firEntry1->end > firEntry2->start) {
                   printf ("%s:%s,%d,%d,%d,%c\nOVERLAPS\n%s:%s,%d,%d,%d,%c\n", argv(1), firEntry1->scaffold.str, firEntry1->start, firEntry1->end, firEntry1->size, firEntry1->strand, argv(2), firEntry2->scaffold.str, firEntry2->start, firEntry2->end, firEntry2->size, firEntry2->strand, );
                   exit (1);
               }
//Print the proper entry
               curEntry = firEntry1;
               printTitleEntry (curEntry, outFile);
               lastPrint = 1;
//Remove the printed entry
               firEntry1 = firEntry1->next;
               freeTitleEntry (curEntry);
               curEntry = firEntry1;
            }
//If list2 is earlier
            if (firEntry2->start < firEntry1->start) {
                curEntry = firEntry1->next;
//Find the last entry before the first entry on list2
                while (curEntry->start < firEntry1->start) {
                    freeTitleEntry (firEntry2);
                    firEntry2 = curEntry;
                    curEntry = firEntry2->next;
               }
//Check that there isn't an impossible overlap
               if (firEntry2->end > firEntry1->start) {
                   printf ("%s:%s,%d,%d,%d,%c\nOVERLAPS\n%s:%s,%d,%d,%d,%c\n", argv(2), firEntry2->scaffold.str, firEntry2->start, firEntry2->end, firEntry2->size, firEntry2->strand, argv(1), firEntry1->scaffold.str, firEntry1->start, firEntry1->end, firEntry1->size, firEntry1->strand, );
                   exit (1);
               }
//Print the proper entry
               curEntry = firEntry2;
               printTitleEntry (curEntry, outFile);
               lastPrint = 2;
//Remove the printed entry
               firEntry2 = firEntry2->next;
               freeTitleEntry (curEntry);
               curEntry = firEntry2;
            }            
//If the first list is empty
        } else if (firEntry1 == NULL) {
            curEntry = firEntry2;
//If last list used was 1, print an entry
            if (lasPrint == 1) {
                printTitleEntry (curEntry, outFile);
                lastPrint = 2;
            }
//Delete the entry
            firEntry2 = firEntry2->next;
            freeTitleEntry (curEntry);
//If the second list is empty
        } else if (firEntry2 == NULL) {
            curEntry = firEntry1;
//If last list used was 2, print an entry
            if (lasPrint == 2) {
                printTitleEntry (curEntry, outFile);
                lastPrint = 1;
            }
//Delete the entry
            firEntry1 = firEntry1->next;
            freeTitleEntry (curEntry);
        }
    }
    return;
}

//Finds the next entry in a file, sets the string to NULL at EOF
void loadTitleEntryList (titleEntry *firEntry, FILE *inFile) {
//Local variables
    char in;
    int count = 0;
    titleEntry *curEntry = NULL, *prevEntry = NULL;
//Find the first entry
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
    curEntry = firEntry;
    prevEntry = firEntry;
//Loop the rest
    while (1) {
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
//Read the scaffold
        in = fgetc (inFile);
        while ((in != ',') && (in != ' ') && (in != '\t') && (in != '\n')) {
            readValueToString (&curEntry->scaffold, in);
            in = fgetc (inFile);
        }
//Read start, end, and size
        fscanf (inFile, "%d%*c%d%*c%d", &curEntry->start, &curEntry->end, &curEntry->size);
        in = fgetc (inFile);
//Lastly, strand
        curEntry->strand = fgetc (inFile);
        in = fgetc (inFile);
//Burn to the next entry
        while (in != '>') {
            in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
        }
//Move to the next entry on the list
        curEntry->next = malloc (sizeof (*curEntry->next));
        initializeTitleEntry (curEntry->next);
        prevEntry = curEntry;
        curEntry = curEntry->next;        
//A counter so the user has some idea of how long it will take
        if (++count % 1000 == 0) {
            printf ("%d titles loaded...\n", count);
        }
    }
//Remove the empty entry
    freeTitleEntry (curEntry);
    prevEntry->next = NULL;
    printf ("%d titles loaded.  ", count);
    return;
}

//Print the contents of an entry
void printTitleEntry (titleEntry *entry, FILE *outFile) {
    
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
