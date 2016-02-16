/* This program separates the singleton entries from novoalign into matching and orphaned.  It takes a novoalign output file as input.  I'll need an incident to use this to be able to clarify WTF I was doing with it though. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{

if (argc != 4)
	{
	printf ("Usage: %s Input_filename Unmatched_match_output_filename Matched_match_output_filename\n", argv[0]);
	return 0;
	}

FILE *input = fopen(argv[1], "r");
FILE *unmatched_out = fopen(argv[2], "w");
FILE *matched_out = fopen(argv[3], "w");
char garbage[250], status[3], line[500];
int i;

printf ("Opening and generating files...\n");
if (input == NULL)
	{
	printf ("Can't open %s\n", argv[1]);
	return 0;
	}
if (unmatched_out == NULL)
	{
	printf ("Can't open %s\n", argv[2]);
	return 0;
	}
if (matched_out == NULL)
	{
	printf ("Can't create %s\n", argv[3]);
	return 0;
	}

printf ("Stage complete.  Sorting entries...\n");
for (i = 0; i < 10; i++)
	{
	fscanf (input, "%[^\n]", garbage);
	}
while (fscanf (input, "%[^\n]%*c", line) != EOF)
	{
	sscanf(line,  "%*s %*s %*s %*s %s", status);
	if (strcmp(status,"NM") == 0)
		{
		fprintf (unmatched_out, "%s\n", line);
		}
	else
		{
		fprintf (matched_out, "%s\n", line);
		}		
	}

fclose (input);
fclose (unmatched_out);
fclose (matched_out);
printf ("Done.\n");

return 0;
}
