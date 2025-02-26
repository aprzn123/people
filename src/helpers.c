// FILES
#include "helpers.h"
#include "constants.h"

// LIBRARIES
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h> // tolower(), isdigit()


// for getNamefilePath and getTimefilePath
// caches results so we only ever allocate once and don't have to worry about freeing
char* namefile = NULL;
char* timefile = NULL;

/* ___________ FUNCTIONS BEGIN ___________ */

void errorMessage(int info) {
    printf("\n┌─┐┌─┐┌─┐┌─┐┬  ┌─┐\n├─┘├┤ │ │├─┘│  ├┤ \n┴  └─┘└─┘┴  ┴─┘└─┘\n\nSyntax:\tpeople\tadd\tforename surname\n\tpeople\tcheck\tforename surname\n\tpeople\tcheck\tall\n\tpeople\tforget\tforename surname\n\tpeople\tforget\tall\n\tpeople\tdays\t[number]\n\nE.g.\tpeople\tadd\tAmy\n\tpeople\tcheck\tJohn Wick\n\tpeople\tdays\t96\n\n");

    if(info == 1)
    {
        // printf("This program was made by Callum Beaney in 2023, distributed under an MIT License.\n\n");
        exit(0);
    }

    exit(1);
}

void setTimespan(int input) {
    FILE *file = fopen(getTimefilePath(), "w");
    if (file == NULL)
    {
        printf("\nNo 'timespan' file present in program directory.\nCreating file with chosen value...\n");
        return;
    }
    fprintf(file, "%d", input);
    fclose(file);
    return;
}

int getTimespan(void){
    int timespanData;
    FILE *file = fopen(getTimefilePath(), "r");
    if (file == NULL)
    {
        // File does not exist, initialize variable to default value
        printf("\nThe file 'timespan' appears to be missing from the data folder.\nCreating 'timespan' data file with default interval of 30 days.\nTo change, use: 'people time [number]\n");
        fclose(file);
        timespanData = 30;
        file = fopen(getTimefilePath(), "w");
        fprintf(file, "%d", timespanData);
        fclose(file);
        return timespanData;
    }
    else
    {
        // File exists, read variable value from file
        fscanf(file, "%d", &timespanData);
        fclose(file);
    }

    return timespanData;
}

char* concatArgumentVector(int argc, char* argv[])
{
    char* buffer = (char *) malloc(100);
    if (buffer == NULL) {
        printf("ERROR: Memory allocation problem.\nThis is likely a system or memory management issue beyond the control of this program.\n");
        exit(1); // The memory allocation operation failed }
    }    
    strcpy(buffer, argv[2]); 
    // 1. if user writes just one name [./people check BatMan] or writes [./people check 'john wick'], strcpy() handles it. 
    // 2. strcpy initialises the above array and appends \0 at the end.
    if (argc > 3) 
    {
        int i = 3;
        while (argv[i] != NULL)
        {
            strcat(buffer, " ");
            strcat(buffer, argv[i]);
            i++;
        }
    }
    return buffer;
}

char* getLowercase(char* userInput)
{
    // allocate memory space the size of the user ARGV
    char* result = malloc(strlen(userInput) + 1);
    for (int i = 0; userInput[i] != '\0'; i++) 
    {
        result[i] = tolower(userInput[i]);
    }
    // printf("TOLOWER   name: %s|\n", userInput);
    // printf("TOLOWER result: %s|\n", result);
    return result;
}

void rewriteDirectory(person* head) 
{
    // here you are basically rebuilding the yellowpages directory file from a linked list you have made an edit to e.g. updating the date of a Person
    FILE* file = fopen(getNamefilePath(), "w");

    person* current = head;
    while (current->next != NULL)
    {
        // printf("date: %s\n",current->next->date);
        char buffer[100];
        snprintf(buffer, sizeof(buffer), "%s,%s\n", current->next->name, current->next->date);
        fprintf(file, "%s", buffer);

        current = current->next;
    }
    fclose(file);
}


void getTodaysDate(int *date)
{
    time_t t = time(NULL);               // get current time data
    struct tm *timeData = localtime(&t); // parse into fields using time.h lib fn

    date[0] = timeData->tm_mday;
    date[1] = timeData->tm_mon + 1;    // add 1 to the tm_mon field
    date[2] = timeData->tm_year + 1900; // add 1900 to tm_year field to get actual year value
    // printf("Today's Date: %i - %i - %i\n", date[2], date[1], date[0]);
}

time_t timeof(int day, int mon, int yr)
{
    struct tm tm;
    time_t tod;

    memset(&tm, 0, sizeof(tm));

    // tm.tm_year = yr - 1900;
    tm.tm_year = yr - 1900;
    tm.tm_mon = mon - 1;
    tm.tm_mday = day;
    tm.tm_hour = 0; // set to midnight to avoid issues with DST crossover or leap seconds
    tm.tm_isdst = -1; // let mktime() determine the correct DST offset

    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", &tm);
    // printf("tm: %s\n", buffer);

    tod = mktime(&tm);

    strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&tod));
    // printf("tod: %s\n", buffer);

    return tod;
}


int compareDates(int* today, int* comparisonDate)
{
    time_t presentDate;
    time_t pastDate;
    time_t difference;

    presentDate = timeof(today[0], today[1], today[2]);
    pastDate = timeof(comparisonDate[0], comparisonDate[1], comparisonDate[2]);
    

    difference = presentDate - pastDate;
    difference /= (24 * 60 * 60);
    // printf("difference: %i\n", (int) difference);

    return (int) difference;
}


void pushPersonToLinkedList(person* head, char* name, char* date) 
{             
    // set current to be the last node in the list
    person* current = head;
    while (current->next != NULL) {
        current = current->next;
    }

    // Add new variable to linked list by creating a new node and copying data into it
    current->next = (person *) malloc(sizeof(person));
    strcpy(current->next->name, name); 
    strcpy(current->next->date, date); 
    current->next->next = NULL;

    // printf("%s, ", current->next->name);
    // printf("%s\n", current->next->date);
}


person* getLinkedListFromNamefile(void)
{
    // Check whether NAMEFILE exists and close if not (e.g. user tries to check a name before adding a name)
    FILE *file = fopen(getNamefilePath(), "r");
    if (file == NULL) {
        printf("\nYou don't have anyone saved to your People List!\nAdd someone using [./people add forename surname] and try again!\n\n");
        fclose(file);
        exit(1);
    }
    fclose(file);

    // Make head of linked list
    person* head = NULL;
    head = (person *) malloc(sizeof(person)); // allocate pointer to malloc'd space the size of a "person" struct
    if (head == NULL) {
        printf("ERROR: Memory allocation problem.\nThis is likely a system or memory management issue beyond the control of this program.\n");
        exit(1); // The memory allocation operation failed 
    }
    head->next = NULL; // if uninitialized, while loop in readPages() can sometimes point to a random memory location & segfault and that would be embarrassing
 
    person* current = head;
    while (current != NULL) {
        printf("%s", current->name);
        current = current->next;
    }

    file = fopen(getNamefilePath(), "r");
    if (file == NULL) {
        printf("Error: failed to open file\n");
        fclose(file);
        exit(1);
    }

    char line[1024];
    char name[50];
    char date[12];

    while (fgets(line, sizeof(line), file) != NULL) {
        // Parse the values from the CSV line using sscanf
        // printf("%s", line);
        if (sscanf(line, "%[^,],%[^\n]", name, date) != 2) {
            printf("Error: invalid input format\n");
            exit(1);
        }
        pushPersonToLinkedList(head, name, date);
    }
    fclose(file);
    return head;
}

void freeList(person * head)
{
   struct node* tmp;

   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }
}


void sortLinkedListByName(person* head) {
    // generic bubble sort
    person *current, *index;
    char tempName[50];
    char tempDate[12];

    if (head == NULL) {
        return;
    }
    else {
        current = head;
        while (current != NULL) {
            index = current->next;

            while (index != NULL) {
                if (strcasecmp(current->name, index->name) > 0) {
                    strcpy(tempName, current->name);
                    strcpy(tempDate, current->date);
                    strcpy(current->name, index->name);
                    strcpy(current->date, index->date);
                    strcpy(index->name, tempName);
                    strcpy(index->date, tempDate);
                }
                index = index->next;
            }
            current = current->next;
        }
    }
}

char* getNamefilePath(void) {
    if (namefile == NULL) {
        char* env = getenv("PEOPLE_NAMEFILE");
        if (env == NULL) {
            char* home = getenv("HOME");
            namefile = malloc(sizeof(char) * (strlen(home) + 10)); // allocate enough size for home directory, plus 10 bytes for "/namefile\0".
            strcpy(namefile, home);
            strcat(namefile, "/namefile");
        } else {
          namefile = env;
        }
    }

    return namefile;
}

char* getTimefilePath(void) {
    if (timefile == NULL) {
        char* env = getenv("PEOPLE_TIMEFILE");
        if (env == NULL) {
            char* home = getenv("HOME");
            timefile = malloc(sizeof(char) * (strlen(home) + 10)); // allocate enough size for home directory, plus 10 bytes for "/timefile\0".
            strcpy(timefile, home);
            strcat(timefile, "/timefile");
        } else {
          timefile = env;
        }
    }

    return timefile;
 }
