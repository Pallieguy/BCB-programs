/* This program analyzes a fasta and calculates several stats concerning the gap content thereof.  It takes a fasta as input. */

//Important includes, alphabetically
#include <math.h> //needs -lm @ compile
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void gatherStats (int minGapSize, FILE *inFile);

//main()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Genome_fasta_filename Minimum_gap_size\n", argv[0]);
        exit (1);
    }
//Main variables
    int userSize;
    FILE *inFile;
//Initialize main variables
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
    sscanf (argv[2], "%d", &userSize);
//Build dynamic linked list of scaf data
    printf ("File found.  Collecting stats...\n");
    gatherStats (userSize, inFile);
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

//Read input and gather stats on each entry
void gatherStats (int minGapSize, FILE *inFile) {
//Local variables
    char in;
    long scafLength = 0, totalScafLength = 0, longestScaf = 0, gapLength = 0, longestGap = 0;
    float scafCount = 1, gaplessScaf = 0, shortestScaf = 0, gapCount = 0, totalGapLength = 0, shortestGap = 0, gapsPerScaf = 0, totalGapsPerScaf = 0, fewestGapsPerScaf = 1000, mostGapsPerScaf = 0, meanScafLength, meanGapLength, meanGapsPerScaf;
//Skip any headers
    in = fgetc (inFile);
    while (in != '>') {
        in = fgetc (inFile);
    }
//Skip the first title
    while (in != '\n') {
        in = fgetc (inFile);
    }
//Process the rest
    while (1) {
        in = fgetc (inFile);
//Break conditions
        if ((ferror (inFile)) || (feof(inFile))) {
            break;
//Check if it's a title and adjsut stats and reset holders involving scaffolds
        }
        if (in == '>') {
            scafCount++;
            if ((fmod (scafCount, 1000)) == 0) {
                printf ("%.0f scaffolds scanned\n", scafCount);
            }
//Adjsut gaps/scaffold stats
            if (gapsPerScaf > 0) {
                if (gapsPerScaf < fewestGapsPerScaf) {
                    fewestGapsPerScaf = gapsPerScaf;
                }
                if (gapsPerScaf > mostGapsPerScaf) {
                    mostGapsPerScaf = gapsPerScaf;
                }
                totalGapsPerScaf += gapsPerScaf;
            } else {
                gaplessScaf++;
            }
//Adjust scaffold stats
            if (shortestScaf == 0) {
                shortestScaf = scafLength;
            } else if (scafLength < shortestScaf) {
                shortestScaf = scafLength;
            }
            if (scafLength > longestScaf) {
                longestScaf = scafLength;
            }
            totalScafLength += scafLength;
            gapsPerScaf = 0;
            scafLength = 0;
//Skip the title line
            while (in != '\n') {
                in = fgetc (inFile);
            }
//Read line until the next title
        } else if (in != '\n') {
            if (in != 'N') {
                scafLength++;
            } else {
//Read entire gap
                while ((in == 'N') || (in =='\n')) {
                    if (in != '\n'){
                        gapLength++;
                        scafLength++;
                    }
                    in = fgetc (inFile);
//If it's at the end of the file
                    if (feof (inFile)) {
                        break;
                    }
                }
//Update scafLength and check it's a big enough gap
                if (gapLength >= minGapSize) {
                    gapCount++;
                    gapsPerScaf++;
//Update as needed
                    if (shortestGap == 0) {
                        shortestGap = gapLength;
                    } else if (gapLength < shortestGap) {
                        shortestGap = gapLength;
                    }
                    if (gapLength > longestGap) {
                        longestGap = gapLength;
                    }
                    totalGapLength += gapLength;
                }
//Reset for next gap and allow normal processing of next character after the gap
                gapLength = 0;
                fseek (inFile, -1, SEEK_CUR);
            }
        }
    }
//Adjust last set of stats, starting with gap/scaffold stats
    if (gapsPerScaf > 0) {
        if (gapsPerScaf < fewestGapsPerScaf) {
            fewestGapsPerScaf = gapsPerScaf;
        }
        if (gapsPerScaf > mostGapsPerScaf) {
            mostGapsPerScaf = gapsPerScaf;
        }
        totalGapsPerScaf += gapsPerScaf;
    } else {
        gaplessScaf++;
    }
//Then adjsut scaffold stats
    if (scafLength < shortestScaf) {
        shortestScaf = scafLength;
    }
    if (scafLength > longestScaf) {
        longestScaf = scafLength;
    }
    totalScafLength += scafLength;
    gapsPerScaf = gapCount / scafCount;
    meanGapLength = totalGapLength / gapCount;
    meanScafLength = totalScafLength / scafCount;
    meanGapsPerScaf = totalGapsPerScaf / (scafCount - gaplessScaf);
//Display Stats
    printf ("========================================\nScaffolds:\t\t\t%.0f\nScaffolds without Gaps:\t\t%.0f\nLongest Scaffold:\t\t%lu\nShortest Scaffold:\t\t%.0f\nMean Scaffold Length:\t\t%.0f\n========================================\nGaps:\t\t\t\t%.0f\nShortest Gap:\t\t\t%.0f\nLongest Gap:\t\t\t%lu\nMean Gap Length:\t\t%.0f\n========================================\nGaps/Scaffold:\t\t\t%f\nFewest Gaps/Scaffold:\t\t%.0f\nMost Gaps/Scaffold:\t\t%.0f\nMean Gaps/Gapped Scaffold:\t%f\n========================================\n", scafCount,gaplessScaf, longestScaf, shortestScaf, meanScafLength, gapCount, shortestGap, longestGap, meanGapLength, gapsPerScaf, fewestGapsPerScaf, mostGapsPerScaf, meanGapsPerScaf);
    return;
}
