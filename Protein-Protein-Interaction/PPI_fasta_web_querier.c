/*This program generates the fasta entries of proteins listed as known interacting and non-intecating pairs.  It takes a positive interacting file and a negative interacting file as input. */

//Standard includes, alphabetically
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Non-standard C includes
#include <curl/curl.h> //Requires "-lcurl" @ compile

//Entry for fasta formatable data
typedef struct proteinEntry {
    char title[7];
    struct proteinEntry *next;
} proteinEntry;

/*THESE ARE NOT MINE*/
enum fcurl_type_e {
  CFTYPE_NONE=0,
  CFTYPE_FILE=1,
  CFTYPE_CURL=2
};
 
struct fcurl_data
{
  enum fcurl_type_e type;     /* type of handle */ 
  union {
    CURL *curl;
    FILE *file;
  } handle;                   /* handle */ 
 
  char *buffer;               /* buffer to store cached data*/ 
  size_t buffer_len;          /* currently allocated buffers length */ 
  size_t buffer_pos;          /* end of data in buffer*/ 
  int still_running;          /* Is background url fetch still in progress */ 
};
 
typedef struct fcurl_data URL_FILE;

/* exported functions */ 
URL_FILE *url_fopen(const char *url,const char *operation);
int url_fclose(URL_FILE *file);
int url_feof(URL_FILE *file);
size_t url_fread(void *ptr, size_t size, size_t nmemb, URL_FILE *file);
char * url_fgets(char *ptr, size_t size, URL_FILE *file);
void url_rewind(URL_FILE *file);

/* we use a global one for convenience */ 
CURLM *multi_handle;
/*END NOT MINE*/

//List of functions, alphabetically
void createFile (FILE **file, char *fName, char perm);
void createProteinEntry (proteinEntry **newEntry);
void insertEntry (char *title, proteinEntry *firEntry);
void loadNegativeProteins (proteinEntry *firEntry, FILE *inFile);
void loadPositiveProteins (proteinEntry *firEntry, FILE *inFile);
void printFastaEntry (char *entry, char *sequence, char matchType, FILE *outFile);
void pullFasta (proteinEntry *entry, char matchType, FILE *outFile);
void pullFastaList (proteinEntry *entry, char matchType, FILE *outFile);

//main ()
int main (int argc, char *argv[]) {
//Call syntax check
    if (argc != 4) {
        printf ("Usage: %s Positive_protein_accession_filename Negative_protein_accession_filename Output_fasta_filename\n", argv[0]);
        exit (1);
    }
//Main variables
    proteinEntry *firProt = NULL;
    FILE *posInFile, *negInFile, *outFile;
//File creation and checks
    printf ("Opening files...\n");
    createFile (&posInFile, argv[1], 'r');
    createFile (&negInFile, argv[2], 'r');
    createFile (&outFile, argv[3], 'w');
//Build dynamic linked list of protein names
    printf ("Files found and created.  Compiling positive protein accession names...\n");
    createProteinEntry (&firProt);
    loadPositiveProteins (firProt, posInFile);
    fclose (posInFile);
//Pull the fasta data from the server
    printf ("Accession names compiled.  Retrieving fastas...\n");
    pullFastaList (firProt, 'I', outFile);
//Build new DLL of protein names
    printf ("Fasta entries retrieved and saved.  Compiling negative protein accession names...\n");
    createProteinEntry (&firProt);
    loadNegativeProteins (firProt, negInFile);
    fclose (negInFile);
//Pull the fasta data from the server
    printf ("Accession names compiled.  Retrieving fastas...\n");
    pullFastaList (firProt, 'N', outFile);
//Close everything and avoid memory leaks
    printf ("Fasta entries retrieved and saved.  Freeing memory and closing files...\n");
    fclose (outFile);
    printf ("Done.\n");
    return 0;
}

//Check if the file was created properly
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
        printf ("Couldn't access %s\n", fName);
        exit (2);
    }
    return;
}

//Create a new node on a fasta entry list
void createProteinEntry (proteinEntry **newEntry) {
    *newEntry = malloc (sizeof (**newEntry));
    (*newEntry)->next = NULL;
    return;
}

//Find the right place and insert the accession title if it isn't already on the list
void insertEntry (char *title, proteinEntry *firEntry) {
//Local variables
    proteinEntry *curEntry, *prevEntry, *newEntry;
    curEntry = firEntry;
//Move through the list if the current title is larger
    while ((curEntry != NULL) && (strcmp (title, curEntry->title) > 0)) {
        prevEntry = curEntry;
        curEntry = curEntry->next;
    }
//If we reached the end of the list, add the node
    if (curEntry == NULL) {
        createProteinEntry (&newEntry);
        prevEntry->next = newEntry;
        curEntry = newEntry;
//If we haven't reached the end of the list
    } else if (strcmp (title, curEntry->title) < 0) {
        createProteinEntry (&newEntry);
//Make a new first entry
        if (curEntry == firEntry) {
            newEntry->next = firEntry;
            firEntry = newEntry;
            curEntry = firEntry;
//Or add the entry between nodes
        } else {
            prevEntry->next = newEntry;
            newEntry->next = curEntry;
            curEntry = newEntry;
        }
    }
//Write the data if a new entry has been made
    if (strcmp (title, curEntry->title) != 0) {
        strcpy (curEntry->title, title);
    }
    return;
}

//Fills the Protein list from the file of positive accession names
void loadPositiveProteins (proteinEntry *firEntry, FILE *inFile) {
//Local Variables
    char line[1000], title1[7], title2[7];
    int loadCount = 0;
//Read first line manually to prime the list
    fscanf (inFile, "%[^\n]%*c", line);
    if (line[0] == '#') {
        fscanf (inFile, "%[^\n]%*c", line);
    }
    sscanf (line, "%*s%s%s", title1, title2);
    strcpy (firEntry->title, title1);
    insertEntry (title2, firEntry);
    while (fscanf (inFile, "%[^\n]%*c", line) != EOF) {
//Parse titles from entry
        sscanf (line, "%*s%s%s", title1, title2);
//Add title to list
        insertEntry (title1, firEntry);
        insertEntry (title2, firEntry);
        if (++loadCount % 1000 == 0) {
            printf ("%d positive entries loaded\n", loadCount);
        }
    }
    return;
}

//Fills the Protein list from the file of negative accession names
void loadNegativeProteins (proteinEntry *firEntry, FILE *inFile) {
//Local Variables
    char line[1000], title1[7], title2[7];
    int loadCount = 0;
//Read first line manually to prime the list
    fscanf (inFile, "%[^\n]%*c", line);
    if (line[0] == '#') {
        fscanf (inFile, "%[^\n]%*c", line);
    }
    sscanf (line, "%s%s", title1, title2);
    strcpy (firEntry->title, title1);
    insertEntry (title2, firEntry);
    while (fscanf (inFile, "%[^\n]%*c", line) != EOF) {
//Parse titles from entry
        sscanf (line, "%s%s", title1, title2);
//Add title to list
        insertEntry (title1, firEntry);
        insertEntry (title2, firEntry);
        if (++loadCount % 1000 == 0) {
            printf ("%d negative entries loaded\n", loadCount);
        }
    }
    return;
}

//Structured output function for fasta entries REBUILD
void printFastaEntry (char *title, char *sequence, char matchType, FILE *outFile) {
//Local variables
    int curLoc = 0;
//Title gets its own line, regardless of length
    fprintf (outFile, ">%s\t%c\n", title, matchType);
//Sequence has a 80 character/line limit
    while (curLoc < strlen (sequence)) {
        if ((curLoc != 0) && (curLoc % 80 == 0)) {
            fprintf (outFile, "\n");
        }
        fprintf (outFile, "%c", sequence[curLoc++]);
    }
    fprintf (outFile, "\n");
    return;
}

//finds and saves the fastas for each protein (assumign on exists)
void pullFasta (proteinEntry *entry, char matchType, FILE *outFile) {
//Local variables
    URL_FILE *handle;
    char buffer[2] = "", url[45] = "http://www.uniprot.org/uniprot/", *sequence, altTitle[11];
    int seqSize = 1, entryCount = 0;
//initialize pointers
    sequence = malloc (seqSize);
    sequence[0] = '\0';
    url[32] = '\0';
//Build full URL
    strcat (url, entry->title);
    strcat (url, ".fasta");
//Open URL
    handle = url_fopen (url, "r");
//If there is data there
    if (handle != NULL) {
//Read it
        while (!url_feof (handle)) {
            url_fread(buffer, 1, 1, handle);
//Skip all title lines as they have useless info (some fastas have multiple entries)
            if (buffer[0] == '>') {
                do {
                    url_fread(buffer, 1, 1, handle);
                } while (buffer[0] != '\n');
//Print the first entry, restart the second, if it's not the first entry
                if (entryCount++ != 0) {
                    sprintf (altTitle, "%s_%d", entry->title, entryCount);
                    printFastaEntry (altTitle, sequence, matchType, outFile);
//Reset sequence to start fresh
                    free (sequence);
                    seqSize = 1;
                    sequence = malloc (seqSize);
                    sequence[0] = '\0';
                }
//Otherwise, grab the fasta data, skipping newline characters
            } else {
                if (buffer[0] != '\n') {
                    seqSize++;
                    sequence = realloc (sequence, seqSize);
                    if (seqSize == 2) {
                        strcpy (sequence, buffer);
                    } else {
                        strcat (sequence, buffer);
                    }
                    sequence[(seqSize - 1)] = '\0';
                }
            }
        }
//Print it
        printFastaEntry (entry->title, sequence, matchType, outFile);
        free (sequence);
        url_fclose (handle);
    }
    return;
}

//Go through a list one by one and pull the associated fasta entry
void pullFastaList (proteinEntry *entry, char matchType, FILE *outFile) {
//Local Variables
    proteinEntry *prevEntry;
    int pullCount = 0;
//Retrieve and free as it goes
    while (entry != NULL) {
        pullFasta (entry, matchType, outFile);
        prevEntry = entry;
        entry = entry->next;
        free (prevEntry);
        if (++pullCount % 1000 == 0) {
            printf ("%d fastas queried\n", pullCount);
        }
    }
    return;
}

/*THESE ARE NOT MINE*/
/* curl calls this routine to get more data */ 
static size_t write_callback(char *buffer,
                             size_t size,
                             size_t nitems,
                             void *userp)
{
  char *newbuff;
  size_t rembuff;
 
  URL_FILE *url = (URL_FILE *)userp;
  size *= nitems;
 
  rembuff=url->buffer_len - url->buffer_pos; /* remaining space in buffer */ 
 
  if(size > rembuff) {
    /* not enough space in buffer */ 
    newbuff=realloc(url->buffer,url->buffer_len + (size - rembuff));
    if(newbuff==NULL) {
      fprintf(stderr,"callback buffer grow failed\n");
      size=rembuff;
    }
    else {
      /* realloc suceeded increase buffer size*/ 
      url->buffer_len+=size - rembuff;
      url->buffer=newbuff;
    }
  }
 
  memcpy(&url->buffer[url->buffer_pos], buffer, size);
  url->buffer_pos += size;
 
  return size;
}
 
/* use to attempt to fill the read buffer up to requested number of bytes */ 
static int fill_buffer(URL_FILE *file, size_t want)
{
  fd_set fdread;
  fd_set fdwrite;
  fd_set fdexcep;
  struct timeval timeout;
  int rc;
 
  /* only attempt to fill buffer if transactions still running and buffer
   * doesnt exceed required size already
   */ 
  if((!file->still_running) || (file->buffer_pos > want))
    return 0;
 
  /* attempt to fill buffer */ 
  do {
    int maxfd = -1;
    long curl_timeo = -1;
 
    FD_ZERO(&fdread);
    FD_ZERO(&fdwrite);
    FD_ZERO(&fdexcep);
 
    /* set a suitable timeout to fail on */ 
    timeout.tv_sec = 60; /* 1 minute */ 
    timeout.tv_usec = 0;
 
    curl_multi_timeout(multi_handle, &curl_timeo);
    if(curl_timeo >= 0) {
      timeout.tv_sec = curl_timeo / 1000;
      if(timeout.tv_sec > 1)
        timeout.tv_sec = 1;
      else
        timeout.tv_usec = (curl_timeo % 1000) * 1000;
    }
 
    /* get file descriptors from the transfers */ 
    curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
 
    /* In a real-world program you OF COURSE check the return code of the
       function calls.  On success, the value of maxfd is guaranteed to be
       greater or equal than -1.  We call select(maxfd + 1, ...), specially
       in case of (maxfd == -1), we call select(0, ...), which is basically
       equal to sleep. */ 
 
    rc = select(maxfd+1, &fdread, &fdwrite, &fdexcep, &timeout);
 
    switch(rc) {
    case -1:
      /* select error */ 
      break;
 
    case 0:
    default:
      /* timeout or readable/writable sockets */ 
      curl_multi_perform(multi_handle, &file->still_running);
      break;
    }
  } while(file->still_running && (file->buffer_pos < want));
  return 1;
}
 
/* use to remove want bytes from the front of a files buffer */ 
static int use_buffer(URL_FILE *file,int want)
{
  /* sort out buffer */ 
  if((file->buffer_pos - want) <=0) {
    /* ditch buffer - write will recreate */ 
    if(file->buffer)
      free(file->buffer);
 
    file->buffer=NULL;
    file->buffer_pos=0;
    file->buffer_len=0;
  }
  else {
    /* move rest down make it available for later */ 
    memmove(file->buffer,
            &file->buffer[want],
            (file->buffer_pos - want));
 
    file->buffer_pos -= want;
  }
  return 0;
}
 
URL_FILE *url_fopen(const char *url,const char *operation)
{
  /* this code could check for URLs or types in the 'url' and
     basicly use the real fopen() for standard files */ 
 
  URL_FILE *file;
  (void)operation;
 
  file = malloc(sizeof(URL_FILE));
  if(!file)
    return NULL;
 
  memset(file, 0, sizeof(URL_FILE));
 
  if((file->handle.file=fopen(url,operation)))
    file->type = CFTYPE_FILE; /* marked as URL */ 
 
  else {
    file->type = CFTYPE_CURL; /* marked as URL */ 
    file->handle.curl = curl_easy_init();
 
    curl_easy_setopt(file->handle.curl, CURLOPT_URL, url);
    curl_easy_setopt(file->handle.curl, CURLOPT_WRITEDATA, file);
    curl_easy_setopt(file->handle.curl, CURLOPT_VERBOSE, 0L);
    curl_easy_setopt(file->handle.curl, CURLOPT_WRITEFUNCTION, write_callback);
 
    if(!multi_handle)
      multi_handle = curl_multi_init();
 
    curl_multi_add_handle(multi_handle, file->handle.curl);
 
    /* lets start the fetch */ 
    curl_multi_perform(multi_handle, &file->still_running);
 
    if((file->buffer_pos == 0) && (!file->still_running)) {
      /* if still_running is 0 now, we should return NULL */ 
 
      /* make sure the easy handle is not in the multi handle anymore */ 
      curl_multi_remove_handle(multi_handle, file->handle.curl);
 
      /* cleanup */ 
      curl_easy_cleanup(file->handle.curl);
 
      free(file);
 
      file = NULL;
    }
  }
  return file;
}
 
int url_fclose(URL_FILE *file)
{
  int ret=0;/* default is good return */ 
 
  switch(file->type) {
  case CFTYPE_FILE:
    ret=fclose(file->handle.file); /* passthrough */ 
    break;
 
  case CFTYPE_CURL:
    /* make sure the easy handle is not in the multi handle anymore */ 
    curl_multi_remove_handle(multi_handle, file->handle.curl);
 
    /* cleanup */ 
    curl_easy_cleanup(file->handle.curl);
    break;
 
  default: /* unknown or supported type - oh dear */ 
    ret=EOF;
    errno=EBADF;
    break;
  }
 
  if(file->buffer)
    free(file->buffer);/* free any allocated buffer space */ 
 
  free(file);
 
  return ret;
}
 
int url_feof(URL_FILE *file)
{
  int ret=0;
 
  switch(file->type) {
  case CFTYPE_FILE:
    ret=feof(file->handle.file);
    break;
 
  case CFTYPE_CURL:
    if((file->buffer_pos == 0) && (!file->still_running))
      ret = 1;
    break;
 
  default: /* unknown or supported type - oh dear */ 
    ret=-1;
    errno=EBADF;
    break;
  }
  return ret;
}
 
size_t url_fread(void *ptr, size_t size, size_t nmemb, URL_FILE *file)
{
  size_t want;
 
  switch(file->type) {
  case CFTYPE_FILE:
    want=fread(ptr,size,nmemb,file->handle.file);
    break;
 
  case CFTYPE_CURL:
    want = nmemb * size;
 
    fill_buffer(file,want);
 
    /* check if theres data in the buffer - if not fill_buffer()
     * either errored or EOF */ 
    if(!file->buffer_pos)
      return 0;
 
    /* ensure only available data is considered */ 
    if(file->buffer_pos < want)
      want = file->buffer_pos;
 
    /* xfer data to caller */ 
    memcpy(ptr, file->buffer, want);
 
    use_buffer(file,want);
 
    want = want / size;     /* number of items */ 
    break;
 
  default: /* unknown or supported type - oh dear */ 
    want=0;
    errno=EBADF;
    break;
 
  }
  return want;
}
 
char *url_fgets(char *ptr, size_t size, URL_FILE *file)
{
  size_t want = size - 1;/* always need to leave room for zero termination */ 
  size_t loop;
 
  switch(file->type) {
  case CFTYPE_FILE:
    ptr = fgets(ptr,size,file->handle.file);
    break;
 
  case CFTYPE_CURL:
    fill_buffer(file,want);
 
    /* check if theres data in the buffer - if not fill either errored or
     * EOF */ 
    if(!file->buffer_pos)
      return NULL;
 
    /* ensure only available data is considered */ 
    if(file->buffer_pos < want)
      want = file->buffer_pos;
 
    /*buffer contains data */ 
    /* look for newline or eof */ 
    for(loop=0;loop < want;loop++) {
      if(file->buffer[loop] == '\n') {
        want=loop+1;/* include newline */ 
        break;
      }
    }
 
    /* xfer data to caller */ 
    memcpy(ptr, file->buffer, want);
    ptr[want]=0;/* allways null terminate */ 
 
    use_buffer(file,want);
 
    break;
 
  default: /* unknown or supported type - oh dear */ 
    ptr=NULL;
    errno=EBADF;
    break;
  }
 
  return ptr;/*success */ 
}
 
void url_rewind(URL_FILE *file)
{
  switch(file->type) {
  case CFTYPE_FILE:
    rewind(file->handle.file); /* passthrough */ 
    break;
 
  case CFTYPE_CURL:
    /* halt transaction */ 
    curl_multi_remove_handle(multi_handle, file->handle.curl);
 
    /* restart */ 
    curl_multi_add_handle(multi_handle, file->handle.curl);
 
    /* ditch buffer - write will recreate - resets stream pos*/ 
    if(file->buffer)
      free(file->buffer);
 
    file->buffer=NULL;
    file->buffer_pos=0;
    file->buffer_len=0;
 
    break;
 
  default: /* unknown or supported type - oh dear */ 
    break;
  }
}
/*END NOT MINE*/
