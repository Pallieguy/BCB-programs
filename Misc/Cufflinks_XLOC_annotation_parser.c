/* This program generates a list of gff entries that match names.  It takes a gff file and cummerBund export csv as input. */

//Standard includes, alphabetically
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//String control made easy
typedef struct string {
    char *str;
    int len;
} string;

//Gff control
typedef struct gffEntry {
    string id;
    string note;
    string dbxref;
    string ontology_term;
    struct gffEntry *next;
} gffEntry;

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createOutputFile (FILE **outFile, char *inName, char *prefix, char *suffix);
void initializeGffEntry (gffEntry *newEntry);
void initializeString (string *newString);
void loadGffList (FILE *inFile, gffEntry *firEntry);
void parseNames (gffEntry *firEntry, FILE *inFile, FILE *outFile);
void readValueToString (string *string, char in);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 3) {
        printf ("Usage: %s Gff_input_filename Csv_list_filename\n", argv[0]);
        exit (1);
    }
//Main varaibles
    char in;
    string title, type;
    FILE *csvInFile = NULL, *gffInFile = NULL, *outFile = NULL;
    gffEntry firEntry;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&gffInFile, argv[1], 'r');
    createFile (&csvInFile, argv[2], 'r');
    createOutputFile (&outFile, argv[2], "Annotated", ".csv");
//Prep the gff list
    printf ("Files opened and created.  Loading gff entries...\n");
    initializeGffEntry (&firEntry);
    loadGffList (gffInFile, &firEntry);
    fclose (gffInFile);
//Parse the titles
    printf ("  Parsing names...\n");
    parseNames (&firEntry, csvInFile, outFile);
//Close everything and free memory
    printf ("  Closing files and freeing memory...\n");
    fclose (csvInFile);
    fclose (outFile);
    printf ("Done.\n");
    return 0;
}

//Create and check a file opening
void createFile (FILE **file, char *fName, char perm) {
    *file = fopen (fName, &perm);
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

//sets minimum values to a gffEntry
void initializeGffEntry (gffEntry *newEntry) {
    initializeString (&(*newEntry).id);
    initializeString (&(*newEntry).note);
    initializeString (&(*newEntry).dbxref);
    initializeString (&(*newEntry).ontology_term);
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

//Loads the gff entries from the file
void loadGffList (FILE *inFile, gffEntry *firEntry) {
//Local variables
    char in;
    int count = 1;
    gffEntry *curEntry = NULL, *prevEntry = NULL;
    curEntry = firEntry;
//Loop the process
    while (1) {
        in = fgetc (inFile);
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
//Skip to the ID section
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
        while (in != '\t') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Make sure it's an ID
        if (in != 'I') {
            printf ("ID not first note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != 'D') {
            printf ("ID not first note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != '=') {
            printf ("ID not first note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
//Load the ID
        while (in != ';') {
            readValueToString (&curEntry->id, in);
            in = fgetc (inFile);
        }
//Make sure the next is name and skip it
        in = fgetc (inFile);
        if (in != 'N') {
            printf ("Name not second note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != 'a') {
            printf ("Name not second note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != 'm') {
            printf ("Name not second note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != 'e') {
            printf ("Name not second note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != '=') {
            printf ("Name not second note of entry %d\n", count);
            exit (2);
        }
        while (in != ';') {
            in = fgetc (inFile);
        }
//Make sure the next is note
        in = fgetc (inFile);
        if (in != 'N') {
            printf ("Note not third note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != 'o') {
            printf ("Note not third note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != 't') {
            printf ("Note not third note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != 'e') {
            printf ("Note not third note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
        if (in != '=') {
            printf ("Note not third note of entry %d\n", count);
            exit (2);
        }
        in = fgetc (inFile);
//Load note
        while (in != ';') {
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            readValueToString (&curEntry->note, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Grab dbxref if there is one
        if (in != '\n') {
            if (in == 'D') {
                in = fgetc (inFile);
                if (in == 'b') {
                    in = fgetc (inFile);
                    if (in == 'x') {
                        in = fgetc (inFile);
                        if (in == 'r') {
                            in = fgetc (inFile);
                            if (in == 'e') {
                                in = fgetc (inFile);
                                if (in == 'f') {
                                    in = fgetc (inFile);
                                    if (in == '=') {
                                        in = fgetc (inFile);
//Load Dbxref
                                        while (in != ';') {
//Break conditions
                                            if (((ferror (inFile)) || (feof (inFile)))) {
                                                break;
                                            }
                                            readValueToString (&curEntry->dbxref, in);
                                            in = fgetc (inFile);
                                        }
                                        in = fgetc (inFile);

                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            free (curEntry->dbxref.str);
            curEntry->dbxref.str = NULL;
        }
//Grab ontology_term if there is one
        if (in != '\n') {
            if (in == 'O') {
                in = fgetc (inFile);
                if (in == 'n') {
                    in = fgetc (inFile);
                    if (in == 't') {
                        in = fgetc (inFile);
                        if (in == 'o') {
                            in = fgetc (inFile);
                            if (in == 'l') {
                                in = fgetc (inFile);
                                if (in == 'o') {
                                    in = fgetc (inFile);
                                    if (in == 'g') {
                                        in = fgetc (inFile);
                                        if (in == 'y') {
                                            in = fgetc (inFile);
                                            if (in == '_') {
                                                in = fgetc (inFile);
                                                if (in == 't') {
                                                    in = fgetc (inFile);
                                                    if (in == 'e') {
                                                        in = fgetc (inFile);
                                                        if (in == 'r') {
                                                            in = fgetc (inFile);
                                                            if (in == 'm') {
                                                                in = fgetc (inFile);
                                                                if (in == '=') {
                                                                    in = fgetc (inFile);
//Load ontology_term
                                                                    while (in != ';') {
//Break conditions
                                                                        if (((ferror (inFile)) || (feof (inFile)))) {
                                                                            break;
                                                                        }
                                                                        readValueToString (&curEntry->ontology_term, in);
                                                                        in = fgetc (inFile);
                                                                    }
                                                                    in = fgetc (inFile);
                                                                }
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } else {
            free (curEntry->ontology_term.str);
            curEntry->ontology_term.str = NULL;
        }
//Check that the line is done
        if ((in != '\n') && !(ferror (inFile)) && !(feof (inFile))) {
            printf ("Entry %d has information after Ontology_term in note\n", count);
            exit (2);
        }
//Load the next entry
        prevEntry = curEntry;
        curEntry->next = malloc (sizeof (*curEntry->next));
        initializeGffEntry (curEntry->next);
        curEntry = curEntry->next;
//A counter so the user has some idea of how long it will take
        if (++count % 10000 == 0) {
            printf ("%d entries processed...\n", count);
        }
    }
//Free the last entry on the list
    free (curEntry->id.str);
    free (curEntry->note.str);
    free (curEntry->dbxref.str);
    free (curEntry->ontology_term.str);
    free (curEntry);
    prevEntry->next = NULL;
    printf ("%d entries processed.", --count);
    return;
}

//Finds and prints the names into a new file
void parseNames (gffEntry *firEntry, FILE *inFile, FILE *outFile) {
//Local variables
    int entry = 1, pos = 0;
    string name, tracking_id, gene_short_name, sample_1, sample_2, status, value_1, value_2, log2_fold_change, test_stat,p_value, q_value, significant;
    char in, gff;
    gffEntry *curEntry = NULL;
//Prime the outfile
    fprintf (outFile, ",tracking_id,gene_short_name, gff_Note, gff_Dbxref,gff_Ontology_term,sample_1,sample_2,status,value_1,value_2,log2_fold_change,test_stat,p_value,q_value,significant\n");
//Skip the header of the csv
    in = fgetc (inFile);
    while (in != '\n') {
        in = fgetc (inFile);
    }
//Loop the process
    while (1) {
//Prep the variables
        in = fgetc (inFile);
//Break conditions
        if (((ferror (inFile)) || (feof (inFile)))) {
            break;
        }
        initializeString (&tracking_id);
        initializeString (&gene_short_name);
        initializeString (&sample_1);
        initializeString (&sample_2);
        initializeString (&status);
        initializeString (&value_1);
        initializeString (&value_2);
        initializeString (&log2_fold_change);
        initializeString (&test_stat);
        initializeString (&p_value);
        initializeString (&q_value);
        initializeString (&significant);
        pos = 0;
//skip entry #
        while (in != ',') {
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load tracking_id
        while (in != ',') {
            readValueToString (&tracking_id, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Check if it's multiple gene_short_names and load accordingly
        if (in == '"') {
//Multiple
            in = fgetc (inFile);
            while (in != '"') {
                readValueToString (&gene_short_name, in);
                in = fgetc (inFile);
            }
            in = fgetc (inFile);
        } else {
//Single
            while (in != ',') {
                readValueToString (&gene_short_name, in);
                in = fgetc (inFile);
            }
        }
        in = fgetc (inFile);
//Load sample_1
        while (in != ',') {
            readValueToString (&sample_1, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load sample_2
        while (in != ',') {
            readValueToString (&sample_2, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load status
        while (in != ',') {
            readValueToString (&status, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load value_1
        while (in != ',') {
            readValueToString (&value_1, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load value_2
        while (in != ',') {
            readValueToString (&value_2, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load log2_fold_change
        while (in != ',') {
            readValueToString (&log2_fold_change, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load test_stat
        while (in != ',') {
            readValueToString (&test_stat, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load p_value
        while (in != ',') {
            readValueToString (&p_value, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load q_value
        while (in != ',') {
            readValueToString (&q_value, in);
            in = fgetc (inFile);
        }
        in = fgetc (inFile);
//Load significant
        while (in != '\n') {
//Break conditions
            if (((ferror (inFile)) || (feof (inFile)))) {
                break;
            }
            readValueToString (&significant, in);
            in = fgetc (inFile);
        }
//Loop to cover multiple gene_short_names
        do {
//Loop specific variable
            initializeString (&name);
            curEntry = firEntry;
//Print the first values
            fprintf (outFile, "%d,%s,", entry, tracking_id.str);
//Parse out the name from gene_short_name
            while (pos < gene_short_name.len) {
                if (gene_short_name.str[pos] == ',') {
                    pos++;
                    break;
                }
                readValueToString (&name, gene_short_name.str[pos++]);
            }
            fprintf (outFile, "%s,", name.str);
//Scan the gff list for a match
            while (strcmp (curEntry->id.str, name.str) != 0) {
                curEntry = curEntry->next;
                if (curEntry == NULL) {
                    break;
                }
            }
//Print the annotation details if there's a match
            if (curEntry != NULL) {
                fprintf (outFile, "\"%s\",", curEntry->note.str);
                if (curEntry->dbxref.str != NULL) {
                    fprintf (outFile, "\"%s\"", curEntry->dbxref.str);
                }
                fprintf (outFile, ",");
                if (curEntry->ontology_term.str != NULL) {
                    fprintf (outFile, "\"%s\"", curEntry->ontology_term.str);
                }
//Otherwise skip it
            } else {
                fprintf (outFile, ",,");
            }
//Then print the rest of the csv
            fprintf (outFile, ",%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n", sample_1.str, sample_2.str, status.str, value_1.str, value_2.str, log2_fold_change.str, test_stat.str, p_value.str, q_value.str, significant.str);
            free (name.str);
        } while (pos < gene_short_name.len);  
//A counter so the user has some idea of how long it will take
        if (++entry % 1000 == 0) {
            printf ("%d entries parsed...\n", entry);
        }
//Reset the strings
        free (tracking_id.str);
        free (gene_short_name.str);
        free (sample_1.str);
        free (sample_2.str);
        free (status.str);
        free (value_1.str);
        free (value_2.str);
        free (log2_fold_change.str);
        free (test_stat.str);
        free (p_value.str);
        free (q_value.str);
        free (significant.str);
    }
    printf ("%d entries parsed.", entry);
    return;
}

//Adds a character to a string, adjsuting size as needed
void readValueToString (string *string, char in) {
    string->str = realloc (string->str, ++(string->len));
    string->str[(string->len - 2)] = in;
    string->str[(string->len - 1)] = '\0';
    return;
}
