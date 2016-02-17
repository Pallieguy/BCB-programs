/* This program generates a gff3 of tRNAscan-SE data.  It takes a TRNAscan output file as input.  May need some edits when it's used again */

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
        printf ("Usage: %s tRNAscan_ouput_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    char line[2000], title[20], type[5], strand;
    int tstart, tend, temp, istart, iend, count = 0;
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
    fscanf (inFile, "%*[^\n]%*c");
    fscanf (inFile, "%*[^\n]%*c");
    fscanf (inFile, "%*[^\n]%*c");
    while (fscanf (inFile, "%[^\n]%*c", line) != EOF) {
        sscanf (line, "%s%*s%d%d%s%*s%d%d%f", title, &tstart, &tend, type, &istart, &iend, &score);
        if (tstart > tend) {
            temp = tstart;
            tstart = tend;
            tend = temp;
            strand = '-';
            if (istart != 0) {
                temp = istart;
                istart = iend;
                iend = temp;
            }
        } else {
            strand = '+';
        }
        count++;
        fprintf (outFile, "%s\ttRNAscan\ttRNA\t%d\t%d\t%.2f\t%c\t.\tID=tRNAScan-tRNA%d-%s\n", title, tstart, tend, score, strand, count, type);
        if (istart != 0) {
            fprintf (outFile, "%s\ttRNAscan\tintron\t%d\t%d\t.\t%c\t.\tParent=tRNAScan-tRNA%d-%s\n", title, istart, iend, strand, count, type);
        }
        fprintf (outFile, "###\n");
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
