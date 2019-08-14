/* This program collects a bunch of counts from a gff3.  It takes a gff3 file as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void initializeString (string *newString);
void readValueToString (string *string, char in);
void reinitializeString (string *string);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 2) {
        printf ("Usage: %s Input_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    FILE *inFile = NULL;
    string source, type;
    char in;
    int maker_count = 0, augustus_count = 0, snap_count = 0, genemark_count = 0, blastn_count = 0, blastx_count = 0, est2genome_count = 0, protein2genome_count = 0, repeatmasker_count = 0, repeatrunner_count = 0, LTRharvest_count = 0, LTR_Finder_count = 0, TRF_count = 0, tRNAscan_count = 0, gene_count = 0, TE_count = 0, count = 0, tblastx_count = 0, cdna2genome_count = 0, mRNA_count = 0, exon_count = 0, CDS_count = 0, five_prime_UTR_count = 0, three_prime_UTR_count = 0, transcript_count = 0;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&inFile, argv[1], 'r');
//Collect stats
    printf ("Files opened.  Parsing stats...\n");
    initializeString (&source);
    initializeString (&type);
    in = fgetc (inFile);
//Automate the rest
    while (1) {
//Skip the title
        while (in != '\t') {
            if (((ferror (inFile)) || (feof (inFile)))) {
                source.str = NULL;
                break;
            }
            in = fgetc (inFile);
        }
//Stop the loop if the list is done
        if (source.str == NULL) {
            break;
        }
//Read the source
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&source, in);
            in = fgetc (inFile);
        }
//Read the type
        in = fgetc (inFile);
        while (in != '\t') {
            readValueToString (&type, in);
            in = fgetc (inFile);
        }
//Burn the rest of the line and reset the strings
        while (in != '\n') {
            in = fgetc (inFile);
        }
//Increase the counts as they appear /*ADD NEW SOURCES HERE */
        if ((strcmp (source.str, "maker") == 0) && (strcmp (type.str, "gene") == 0)) {
            maker_count++;
        } else if ((strcmp (source.str, "augustus_masked") == 0) && (strcmp (type.str, "match") == 0)) {
            augustus_count++;
        } else if ((strcmp (source.str, "snap_masked") == 0) && (strcmp (type.str, "match") == 0)) {
            snap_count++;
        } else if ((strcmp (source.str, "genemark_masked") == 0) && (strcmp (type.str, "match") == 0)) {
            genemark_count++;
        } else if ((strcmp (source.str, "blastn") == 0) && (strcmp (type.str, "expressed_sequence_match") == 0)) {
            blastn_count++;
        } else if ((strcmp (source.str, "blastx") == 0) && (strcmp (type.str, "protein_match") == 0)) {
            blastx_count++;
        } else if ((strcmp (source.str, "tblastx") == 0) && (strcmp (type.str, "translated_nucleotide_match") == 0)) {
            tblastx_count++;
        } else if ((strcmp (source.str, "est2genome") == 0) && (strcmp (type.str, "expressed_sequence_match") == 0)) {
            est2genome_count++;
        } else if ((strcmp (source.str, "protein2genome") == 0) && (strcmp (type.str, "protein_match") == 0)) {
            protein2genome_count++;
        } else if ((strcmp (source.str, "cdna2genome") == 0) && (strcmp (type.str, "expressed_sequence_match") == 0)) {
            cdna2genome_count++;
        } else if ((strcmp (source.str, "repeatmasker") == 0) && (strcmp (type.str, "match") == 0)) {
            repeatmasker_count++;
        } else if ((strcmp (source.str, "repeatrunner") == 0) && (strcmp (type.str, "protein_match") == 0)) {
             repeatrunner_count++;
        } else if ((strcmp (source.str, "LTRharvest") == 0) && (strcmp (type.str, "repeat_region") == 0)) {
            LTRharvest_count++;
        } else if ((strcmp (source.str, "LTR_Finder") == 0) && (strcmp (type.str, "repeat_region") == 0)) {
            LTR_Finder_count++;
        } else if ((strcmp (source.str, "TRF") == 0) && (strcmp (type.str, "repeat_region") == 0)) {
            TRF_count++;
        } else if ((strcmp (source.str, "tRNAscan") == 0) && (strcmp (type.str, "tRNA") == 0)) {
            tRNAscan_count++;
        } else if (strcmp (type.str, "gene") == 0) {
            gene_count++;
        } else if (strcmp (type.str, "transcricpt") == 0) {
            transcript_count++;
        } else if (strcmp (type.str, "mRNA") == 0) {
            mRNA_count++;
        } else if (strcmp (type.str, "exon") == 0) {
            exon_count++;
        } else if (strcmp (type.str, "CDS") == 0) {
            CDS_count++;
        } else if (strcmp (type.str, "five_prime_UTR") == 0) {
            five_prime_UTR_count++;
        } else if (strcmp (type.str, "three_prime_UTR") == 0) {
            three_prime_UTR_count++;
        } else if (strcmp (type.str, "transposable_element") == 0) {
            TE_count++;
        }
//A counter so the user has some idea of how long it will take
        if (++count % 100000 == 0) {
            printf ("%d entries processed...\n", count);
        }
        reinitializeString (&source);
        reinitializeString (&type);
    }
//Display the stats
    printf ("%d entries process.  All stats Parsed.\n\t###MAKER Matches###\nMAKER Consensus Genes:\t%d\nAugustus Matches:\t%d\nsnap Matches:\t\t%d\nGenemark Matches:\t%d\nBLASTn Matches:\t\t%d\nBLASTx Matches:\t\t%d\ntBLASTx Matches:\t%d\nest2genome Matches:\t%d\nprotein2genome Matches:\t%d\ncdna2genome Matches:\t%d\nRepeatMasker Matches:\t%d\nRepeatrunner Matches:\t%d\n\t###Other Annotations###\nLTRharvest Matches:\t%d\nLTR_Finder Matches:\t%d\nTRF Matches:\t\t%d\ntRNAscan Matches:\t%d\n\t###Generic Matches###\nGenes:\t\t\t%d\nTrasnscripts:\t\t%d\nmRNAs:\t\t\t%d\nExons:\t\t\t%d\nCDSs:\t\t\t%d\n5' UTRs:\t\t%d\n3' UTRs:\t\t%d\nTransposable Elements:\t%d\n", count, maker_count, augustus_count, snap_count, genemark_count, blastn_count, blastx_count, tblastx_count, est2genome_count, protein2genome_count, cdna2genome_count, repeatmasker_count, repeatrunner_count, LTRharvest_count, LTR_Finder_count, TRF_count, tRNAscan_count, gene_count, transcript_count, mRNA_count, exon_count, CDS_count, five_prime_UTR_count, three_prime_UTR_count, TE_count);
//Close everything and free memory
    free (source.str);
    free (type.str);
    fclose (inFile);
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

//Resets a used string to an empty status
void reinitializeString (string *string) {
    free (string->str);
    initializeString (&(*string));
    return;
}
