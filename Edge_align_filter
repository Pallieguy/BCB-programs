/* According to notes this was used for novoalign output filtering.  If it ever gets used again it needs to be updated. */

//Standard includes, alphabetically
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 4) {
        printf ("Usage: %s Input_filename Minimum_distance_from_contig_ends\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL, *outFile = NULL;
    int min_dist = strtol(argv[3], NULL, 10), offset, nucleotides, i;
    char line[500], title[45], sequence[52], contig[50], nucleotides_string[10];
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    createOutputFile (&outFile, argv[1]);
//Generate the fasta entry from the novoalign output
    printf ("Generating fasta...\n");
    while (fscanf (inFile, "%[^\n]%*c", line) != EOF) {
//Looks like a lot of data on one line /*EDIT HERE IF REBUILD*/
        sscanf (line, "%s%*s%s%*s%*s%*s%*s%s%d", title, sequence, contig, &offset);
        if (offset > min_dist) {
            sscanf (contig, "%*[^'_']%*c%[^'_']", nucleotides_string);
            nucleotides = strtol (nucleotides_string, NULL, 10);
            if ((nucleotides - offset) > min_dist) {
                fprintf (outFile, "%s\n%s\n", title, sequence);
            }
        }
    }
//Close everything
    fclose (inFile);
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
    while (inName[i] != '\0') {
        fileName[j++] = inName[i++];
    }
    fileName[j] = '\0';
//Allow for the added text EDIT THIS AT COPY
    outTitle = malloc (j + 7);
    outTitle[0] = '\0';
    strcat (outTitle, fileName);
    strcat (outTitle, ".fasta");
    free (fileName);
//Create it
    createFile (*(&outFile), outTitle, 'w');
    free (outTitle);
    return;
}
