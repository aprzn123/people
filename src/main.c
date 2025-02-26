#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <ctype.h>

#include "helpers.h"
#include "constants.h"

// TODO: Windows support
// TODO: system("clear"); // or "cls" for Windows

int main(int argc, char *argv[])
{
/* 
    DELETE DIRECTORY: people forget all 
*/    
    if (argc == 3 && strcmp(argv[1], "forget") == 0 && strcmp(argv[2], "all") == 0) 
    {
        int purge = remove(getNamefilePath());
        if(purge == 0) {
            printf("\nPeople List deleted successfully.\nA new one will be generated when you next use [./people add _____]\n\n");
            return 0;
        } else {
            printf("\nError: unable to delete the file\n");
            return 1;
        }
    }
/* 
    REMOVE PERSON FROM DIRECTORY: people forget name name 
*/    
    else if (argc >= 3 && strcmp(argv[1], "forget") == 0) 
    {
        char* userInputtedName;
        char* lowercaseName;
        userInputtedName = concatArgumentVector(argc, argv);
        lowercaseName = getLowercase(userInputtedName);

        // Check whether NAMEFILE exists -- if exists, cycle a linkedlist built from it and check whether Name To Delete exists.
        person* directory = getLinkedListFromNamefile();
        FILE* file = fopen(getNamefilePath(), "r");
        if (file != NULL) {
            person* current = directory; //head node for loop-checking
            int found = 0;
            while(current->next != NULL) 
            {
                char* lowercaseLinkedListName;
                lowercaseLinkedListName = getLowercase(current->next->name);
                if(strcmp(lowercaseName, lowercaseLinkedListName) == 0)
                {   
                    // name is in the directory
                    found = 1;
                    free(lowercaseLinkedListName);
                    break;   
                }
                current = current->next; // move pointer onto next node. 
                // Without this pointer relocation your terminal goes into an endless loop and, suffering from the flu, you stay up exhausted and confused till 2AM repeatedly failing to notice this obvious error and feel a shame that though viewed from afar may seem to be but a tiny twinkling speck in the cavernous abyss of the great night sky, is in actuality a celstial behemoth of furiously combusting solar disgrace that will burn deep within your soul forever, consuming it until finally the last oxidizable molecule of your spirit is but a pitiful ember drifting in the wind along the shale afore the gloaming. 
            }
            if (found != 1) {
                printf("\nERROR: this name was not found in your user list. Use [./people check all] to check your user list.\n\n");
                free(userInputtedName);
                free(lowercaseName);
                freeList(directory);
                fclose(file);
                return 1;
            }
        }
        else if (file == NULL) {
            printf("\n\nERROR: Your People List file does not exist. Use [./people add forename surname] to create and add to your People List.\n\n");
            free(userInputtedName);
            free(lowercaseName);
            fclose(file);
            return 1;
        }
        fclose(file);

        // if the program gets here, the user's inputted an existing name + the People List NAMEFILE exists.
        // Rewrite the directory to the NAMEFILE by excluding the node with the unwanted matching ->name
        file = fopen(getNamefilePath(), "w");
        person* current = directory;
        while (current->next != NULL)
        {
            char* lowercaseLinkedListName;
            lowercaseLinkedListName = getLowercase(current->next->name);
            if (strcmp(lowercaseName, lowercaseLinkedListName) != 0)
            {
                char buffer[100];
                snprintf(buffer, sizeof(buffer), "%s,%s\n", current->next->name, current->next->date);
                fprintf(file, "%s", buffer);
            }

            current = current->next;
        }
        fclose(file);
        freeList(directory);
        printf("\n%s has been removed from your People List\n\n", userInputtedName);
        return 0;
    }
/* 
    SET TIMESPAN: people days [number] 
*/
    if (argc == 3 && strcmp(argv[1], "days") == 0) 
    {
        char *rawInput = argv[2];
        for (int i = 0; i < strlen(rawInput); i++)
        {
            if (!isdigit(rawInput[i]))
            {
                printf("\nERROR: %s is not a valid number", rawInput);
                printf("\nUsage: e.g.\tpeople\tdays\t69 [set timespan to 69 days]\n\n");
                exit(1);
            }
        }
        int userInput = atoi(rawInput);
        setTimespan(userInput);
        printf("\nInterval to compare dates set to: %d days \n\n", userInput);
        return 0;
    }
/* 
    ADD PERSON TO DIRECTORY: people add name name
*/
    else if ((argc >= 3) && strcmp(argv[1], "add") == 0) 
    {
        char* userInputtedName;
        userInputtedName = concatArgumentVector(argc, argv);
        char* lowercaseName;
        lowercaseName = getLowercase(userInputtedName);

        // Check whether NAMEFILE exists, if it does cycle through linked-list and check whether the Name-To-Add already exists
        FILE* file = fopen(getNamefilePath(), "r");
        if (file != NULL) {
            person* directory = getLinkedListFromNamefile();
            person* current = directory; // head node
            while (current->next != NULL) 
            {
                char* lowercaseLinkedListName;
                lowercaseLinkedListName = getLowercase(current->next->name);
                if(strcmp(lowercaseName, lowercaseLinkedListName) == 0)
                {          
                    printf("\n\t%s is already in your People List!\nTo check: [ ./people check %s ]\n\n", userInputtedName, userInputtedName);
                    free(lowercaseLinkedListName);
                    free(userInputtedName);
                    freeList(directory);
                    fclose(file);
                    return 1;
                }
                current = current->next;
            }
        }
        fclose(file);

        // If the program gets to here, then the user's inputted a new name. 
        // If the file == NULL, the fopen below is going to initialise that namefile anyway.
        int dateToday[3];
        getTodaysDate(dateToday);
        char buffer[100];

        file = fopen(getNamefilePath(), "a");
        snprintf(buffer, sizeof(buffer), "%s,%i/%i/%i\n", userInputtedName, dateToday[0], dateToday[1], dateToday[2]);
        fprintf(file, "%s", buffer);
        fclose(file);

        printf("\nAdded %s to your People List\n\n", userInputtedName);
        free(userInputtedName);
        free(lowercaseName);
        return 0;
    }
/* 
    CHECK ALL PEOPLE IN DIRECTORY: people check [all] 
*/     
    else if (argc == 3 && strcmp(argv[1], "check") == 0 && strcmp(argv[2], "all") == 0) 
    {
        // cycle through linked-list & compare lowercased names 
        person* directory = getLinkedListFromNamefile();        
        person* current = directory; // head node

        sortLinkedListByName(current);

        while (current->next != NULL)  {
            int dateToday[3];
            getTodaysDate(dateToday);

            // Make a copy of the date without touching the struct
            char* dateCopy = strdup(current->next->date);
            if (dateCopy == NULL) {
                printf("ERROR: Memory allocation error.\n");
                exit(1);
            }

            //strtok() modifies the string it is called on and replaces the delimiter with a null terminator. 
            char* token = strtok(dateCopy, "/");

            int comparator[3];                
            int i = 0;

            while (token != NULL) {
                // parse node date (e.g. 19/9/1999) into comparator
                comparator[i] = atoi(token);
                i++;
                token = strtok(NULL, "/");
            }

            int daysSinceLastChecked = compareDates(dateToday, comparator);
            int elapsedCheck = getTimespan();

            // This just spaces the names appropriately
            int len = strlen(current->next->name);
            if (len >= 23) {
                printf("\n%s\t- last checked\t%i\tdays ago", current->next->name, daysSinceLastChecked);
            }
            else if (len >= 16) {
                printf("\n%s\t- last checked\t%i\tdays ago", current->next->name, daysSinceLastChecked);
            } else if (len < 8) {
                printf("\n%s\t\t\t- last checked\t%i\tdays ago", current->next->name, daysSinceLastChecked);
            } else {
                printf("\n%s\t\t- last checked\t%i\tdays ago", current->next->name, daysSinceLastChecked);
            }

            if (daysSinceLastChecked > elapsedCheck) {
                printf("  ! IMPORTANT");
            }
            current = current->next; 
        }

        // ask user if they want to reset dates
        printf("\n\nReset number of days passed for ALL entries to 0?\nY/N: ");
        char yesNo;
        scanf(" %[^\n]%*c", &yesNo); // get a single char
        yesNo = toupper(yesNo);
        if (yesNo == 'Y') {
            int dateToday[3];
            getTodaysDate(dateToday);
            person* current = directory; 
            while (current->next != NULL) 
            {   // Reset date in linked list
                snprintf(current->next->date, sizeof(current->next->date), "%i/%i/%i", dateToday[0], dateToday[1], dateToday[2]);
                current = current->next; 
            }
            rewriteDirectory(directory); // write linked list to NAMEFILE
            printf("Changed lookup date for ALL to today's date.\n\n");    
        } else {
            rewriteDirectory(directory); 
        }
        
        printf("\n");
        freeList(directory);
        return 0;
    }
/* 
    CHECK A SPECIFIC PERSON: people check forename surname 
*/
    else if (strcmp(argv[1], "check") == 0 && argc >= 3) 
    {   
        // Ensure they aren't fudging a [./people check all]
        if (strcmp(argv[2], "all") == 0) {
            printf("\nERROR: To check for all people in the list, use: [./people check all]\n\n");
            return 1;
        }

        char* userInputtedName;
        char* lowercaseName;
        userInputtedName = concatArgumentVector(argc, argv);
        lowercaseName = getLowercase(userInputtedName);
        
        // cycle through linked-list & compare lowercased names 
        person* directory = getLinkedListFromNamefile();
        person* current = directory; // head node

        while (current->next != NULL) 
        {
            char* lowercaseLinkedListName;
            lowercaseLinkedListName = getLowercase(current->next->name);
            
            if(strcmp(lowercaseName, lowercaseLinkedListName) == 0)
            {  
                int dateToday[3];
                getTodaysDate(dateToday);

                // Make a copy of the date without touching the struct
                char* dateCopy = strdup(current->next->date);
                if (dateCopy == NULL) {
                    printf("ERROR: Memory allocation error.\n");
                    exit(1);
                }

                //strtok() modifies the string it is called on and replaces the delimiter with a null terminator. 
                char* token = strtok(dateCopy, "/");

                int comparator[3];                
                int i = 0;

                while (token != NULL) {
                    // parse node date (e.g. 19/9/1999) into comparator
                    comparator[i] = atoi(token);
                    i++;
                    token = strtok(NULL, "/");
                }

                int daysSinceLastChecked = compareDates(dateToday, comparator);
    
                printf("\n%s - last checked %i days ago\n\n", current->next->name, daysSinceLastChecked);

                // Check if user wants to reset date 
                printf("Reset number of days passed to 0?\nY/N: ");
                char yesNo;
                scanf(" %[^\n]%*c", &yesNo); // get a single char
                yesNo = toupper(yesNo);
                if (yesNo == 'Y') {
                    // Reset date in linked list
                    snprintf(current->next->date, sizeof(current->next->date), "%i/%i/%i", dateToday[0], dateToday[1], dateToday[2]);

                    rewriteDirectory(directory); // write linked list to NAMEFILE
                    printf("\nChanged lookup date for %s to today's date.\n\n", userInputtedName);    
                }
                
                free(lowercaseName);
                free(userInputtedName);
                free(lowercaseLinkedListName);
                freeList(directory);
                return 0;
            }
            current = current->next;
        }

        // If program gets here, the name user is checking isn't in the list.
        printf("\nERROR: \'%s\' not found in your People List.\nTo add them to your list, use: ./people add \'%s\'\n\n", userInputtedName, userInputtedName);
        free(lowercaseName);
        free(userInputtedName);
        freeList(directory);
        return 1;
    }
/*  
    // ERROR MESSAGE: User doesn't know how to use the program and wants to know
*/
    else if (argc == 2 && strcmp(argv[1], "info") == 0 ) {
        errorMessage(1);
    }
/*  
    // ERROR MESSAGE: User doesn't know how to use the program and probably needs help
*/
    else {
        errorMessage(0);
    }

    // the program should never get here
    exit(2);
}
