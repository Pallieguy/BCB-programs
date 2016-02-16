/* This program generates a gff3 of TRF data.  It takes a TRF output file as input. May need some edits when used next*/

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
    if (argc != 2) {
        printf ("Usage: %s TRF_output_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    char line[20000], oldline[20000], title[20], sequence[1000];
    int start, end, size, count = 0, tempend;
    float score;
    FILE *inFile = NULL, *outFile = NULL;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Parse out entry data
    printf ("Generating entries...\n");
    fprintf (outFile, "##gff-version   3\n");
/*REBUILD THIS*/
    while (fscanf (inFile, "%[^\n]%*c", line) != EOF) {
        if (strcmp (oldline, line) == 0) {
            fscanf (inFile, "%*c");
        } else {
            if (line[0] == 'S') {
                sscanf (line, "%*s%s", title);
            }
            if ((line[0] == '1') || (line[0] == '2') || (line[0] == '3') || (line[0] == '4') || (line[0] == '5') || (line[0] == '6') || (line[0] == '7') || (line[0] == '8') || (line[0] == '9')) {
                sscanf (line, "%d%d%d%*s%*s%*s%*s%f", &start, &end, &size, &score);
                fprintf (outFile, "%s\tTRF\trepeat_region\t%d\t%d\t%.2f\t?\t.\tID=TRF_repeat%d\n", title, start, end, score, ++count);
                while (start < end) {
                    if ((start + size) < end) {
                        tempend = (start + size);
                    } else {
                        tempend = end;
                    }
                    fprintf (outFile, "%s\tTRF\ttandem_repeat\t%d\t%d\t.\t?\t.\tParent=TRF_repeat%d\n", title, start, tempend, count);
                    start = ++tempend;
                }
                fprintf (outFile, "###\n");
            }
        }
        strcpy (oldline, line);
    }
//Close everything and free memory
    printf ("Entries generated.  Closing files and freeing memory...\n");
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
