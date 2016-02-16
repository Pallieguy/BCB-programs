/* This program compares Adrian's GtoN program with mine.  It takes two corrected fasta files as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s AP_input_filename SM_input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    char inAP, inSM;
    int count = 0, position = 0;
    FILE *inFileAP = NULL, *inFileSM = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFileAP, argv[1], 'r');
    createFile (&inFileSM, argv[2], 'r');
    createFile (&outFile, "AP-SM_mismatches.txt", 'w');
//Character by character comparisson
    printf ("Files opened.  Comparing entries...\n");\
    while (1) {
        inAP = fgetc (inFileAP);
        inSM = fgetc (inFileSM);
//Stop conditions
        if (((ferror (inFileAP)) || (feof (inFileAP))) || ((ferror (inFileSM)) || (feof (inFileSM)))) {
            break;
        }
        position++;
//A counter so the user has some idea of how long it will take
        if (inAP == '>') {
            if (++count % 1000 == 0) {
                printf ("%d entries processed...\n", count);
            }
        }
//If they mismatch print where it happened
        if (inAP != inSM) {
            fprintf (outFile, "Position:%d AP:%c SM:%c\n", position, inAP, inSM);        
        }
    }
//Close everything and free memory
    printf ("%d entries processed.  Closing files and freeing memory\n", count);
    fclose (inFileAP);
    fclose (inFileSM);
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
