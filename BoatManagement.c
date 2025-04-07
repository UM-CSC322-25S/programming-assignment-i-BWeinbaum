#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//Levy definitions
#define SLIP_COST 12.5
#define LAND_COST 14
#define TRAILOR_COST 25
#define STORAGE_COST 11.2

//Maximum definitions
#define MAX_CSV_LINE_LENGTH 1024
#define MAX_BOATS 120
#define MAX_BOAT_NAME_LENGTH 127
#define MAX_BOAT_LENGTH 100
#define TRAILOR_TAG_LENGTH 6

//Data types
typedef char BoatName[MAX_BOAT_NAME_LENGTH];
typedef char CSVLine[MAX_CSV_LINE_LENGTH];
typedef char TrailorTag[TRAILOR_TAG_LENGTH];
typedef enum {
    slip,
    land,
    trailor,
    storage,
    no_place
} PlaceType;
typedef union {
    int slipID;
    char landID;
    TrailorTag trailorTag;
    int storageSpace;
} PlaceID;
typedef struct {
    BoatName name;
    int length;
    PlaceType placeType;
    PlaceID placeID;
    double moneyOwed;
} Boat;
typedef Boat *BoatPointer;
typedef BoatPointer BoatPointers[MAX_BOATS];


//----Convert a string to a place
PlaceType StringToPlaceType(char *PlaceString) {
    if (!strcasecmp(PlaceString,"slip")) {
        return(slip);
    }
    if (!strcasecmp(PlaceString,"land")) {
        return(land);
    }
    if (!strcasecmp(PlaceString,"trailor")) {
        return(trailor);
    }
    if (!strcasecmp(PlaceString,"storage")) {
        return(storage);
    }
    return(no_place);
}

//----Convert a place to a string
char *PlaceToString(PlaceType Place) {
    switch (Place) {
        case slip:
            return("slip");
        case land:
            return("land");
        case trailor:
            return("trailor");
        case storage:
            return("storage");
        case no_place:
            return("no_place");
        default:
            printf("How the faaark did I get here?\n");
            exit(EXIT_FAILURE);
            break;
    }
}

/*Function to add a new boat to the boat pointers array based on the
information from a line in a CSV file*/
void addBoatFromCSVLine(BoatPointers boatPtrs,int *count,CSVLine line) {
    /*Check that max boats has not been reached and memory can be allocated for
    the new boat.*/
    if (*count == MAX_BOATS) {
        printf("Maximum boats reached.");
	return;
    }

    boatPtrs[*count] = (BoatPointer) malloc(sizeof(Boat));
    if (boatPtrs[*count] == NULL) {
        printf("Failed to allocate memory for new boat.");
	return;
    }

    //Interpret CSV line
    BoatName name;
    int length;
    PlaceType placeType;
    PlaceID placeID; 
    double moneyOwed;

    char *token = strtok(line,",");
    if (token == NULL) {
        printf("Invalid syntax in CSV file.");
	free(boatPtrs[*count]);
	return;
    }
    strcpy(name,token);
    token = strtok(NULL,",");
    if (token == NULL) {
        printf("Invalid syntax in CSV file.");
	free(boatPtrs[*count]);
	return;
    }
    length = atoi(token);
    token = strtok(NULL,",");
    if (token == NULL) {
        printf("Invalid syntax in CSV file.");
	free(boatPtrs[*count]);
	return;
    }
    placeType = StringToPlaceType(token);
    token = strtok(NULL,",");
    if (token == NULL) {
        printf("Invalid syntax in CSV file.");
	free(boatPtrs[*count]);
	return;
    }
    if (placeType == slip)
        placeID.slipID = atoi(token);
    else if (placeType == land)
        placeID.landID = token[0];
    else if (placeType == trailor)
        strcpy(placeID.trailorTag,token);
    else if (placeType == storage)
        placeID.storageSpace = atoi(token);
    token = strtok(NULL,",");
    if (token == NULL) {
        printf("Invalid syntax in CSV file.");
	free(boatPtrs[*count]);
	return;
    }
    moneyOwed = atof(token);

    //Transfer data to new boat
    strcpy(boatPtrs[*count]->name,name);
    boatPtrs[*count]->length = length;
    boatPtrs[*count]->placeType = placeType;
    if (placeType != trailor) {
        boatPtrs[*count]->placeID = placeID;
    } else {
        strncpy(boatPtrs[*count]->placeID.trailorTag,placeID.trailorTag,sizeof(
boatPtrs[*count]->placeID.trailorTag)-1);
        boatPtrs[*count]->placeID.trailorTag[sizeof(boatPtrs[*count]->placeID
.trailorTag)-1] = '\0';
    }
    boatPtrs[*count]->moneyOwed = moneyOwed;

    (*count)++;
}

/* Comparison method for qsort of BoatPointers. */
int compareBoats(const void *a,const void *b) {
    const BoatPointer *boatPtrA = (const BoatPointer*) a;
    const BoatPointer *boatPtrB = (const BoatPointer*) b;
    return strcmp((*boatPtrA)->name,(*boatPtrB)->name);
}

/* Function to sort the boats by name (alphabetical order), then display all of
 * their information.*/
void printInventory(BoatPointers boatPtrs,const int count) { 
    //Sort boatPtrs
    qsort(boatPtrs,count,sizeof(BoatPointer),compareBoats);
	
    TrailorTag placeIDOutput; //string with max length 6
    int Index;
    for (Index = 0;Index < count;Index++) {
        //Interpret the place type and ID
	if (boatPtrs[Index]->placeType == slip) {
            sprintf(placeIDOutput,"# %d",boatPtrs[Index]->placeID.slipID);
        } else if (boatPtrs[Index]->placeType == land) {
            placeIDOutput[0] = boatPtrs[Index]->placeID.landID;
	    placeIDOutput[1] = '\0';
        } else if (boatPtrs[Index]->placeType == trailor) {
            strcpy(placeIDOutput,boatPtrs[Index]->placeID.trailorTag);
	} else if (boatPtrs[Index]->placeType == storage) {
            sprintf(placeIDOutput,"# %d",boatPtrs[Index]->placeID.storageSpace);
        } else {
            printf("Invalid place type.");
	    return;
        }


	//Print all of the boat info
        printf("%-21s %d\' %7s %6s   Owes $%7.2lf\n",boatPtrs[Index]->name,
boatPtrs[Index]->length,PlaceToString(boatPtrs[Index]->placeType),
placeIDOutput,boatPtrs[Index]->moneyOwed);
    }
}

/* Function to find the index of a specific boat in the BoatPointers array by
 * name. */
int findBoatIndex(BoatPointers boatPtrs,const int count,BoatName name) {
    int boatIndex;
    for (boatIndex = 0;boatIndex < count;boatIndex++) {
        if (strcasecmp(boatPtrs[boatIndex]->name,name) == 0)
            break;
    }
    
    //Handle if boat not found by name
    if (boatIndex == count) {
        printf("No boat with that name\n");
	return -1;
    }

    return boatIndex;
}
/* Function for removing a boat from the array based on its name. If the name
 * is not found, this function will inform the user. */
void removeBoat(BoatPointers boatPtrs,int *count,BoatName name) {
    int boatIndex = findBoatIndex(boatPtrs,*count,name);
    if (boatIndex == -1)
        return;

    //Assuming name is found, remove boat
    free(boatPtrs[boatIndex]);
    int Index;
    for (Index = boatIndex;Index < *count;Index++) {
        boatPtrs[Index] = boatPtrs[Index+1];
    }
    boatPtrs[*count-1] = NULL;
    (*count)--;
}

void acceptPayment(BoatPointers boatPtrs,int *count,BoatName name) {
    int boatIndex = findBoatIndex(boatPtrs,*count,name);
    if (boatIndex == -1)
        return;

    //Assuming name is found, request amount to be paid
    double payment;
    printf("Please enter the amount to be paid                       : ");
    scanf("%lf",&payment);
    getchar(); //discard newline char

    //Do not accept payment if it is above what is owed
    double moneyOwed = boatPtrs[boatIndex]->moneyOwed;
    if (payment > moneyOwed) {
        printf("That is more than the amount owed, $%.2f\n",moneyOwed);
	return;
    } else {
        boatPtrs[boatIndex]->moneyOwed -= payment;
    }
}

/* Function for updating the amount owed by each boat to the marina assuming
 * a new month has passed. These rates are determined by the levy definition
 * constants listed at the top of this C file. */
void updateAmounts(BoatPointers boatPtrs,const int count) {
    int Index;
    int length;
    PlaceType placeType;
    for (Index = 0;Index < count;Index++) {
        length = boatPtrs[Index]->length;
        placeType = boatPtrs[Index]->placeType;

        if (placeType == slip) {
            boatPtrs[Index]->moneyOwed += (length*SLIP_COST);
	} else if (placeType == land) {
            boatPtrs[Index]->moneyOwed += (length*LAND_COST);
        } else if (placeType == trailor) {
            boatPtrs[Index]->moneyOwed += (length*TRAILOR_COST);
        } else if (placeType == storage) {
            boatPtrs[Index]->moneyOwed += (length*STORAGE_COST);
        }
    }
}

int main(int argc,char *argv[]) {
    
    //Start by importing data from the CSV file
    if (argc != 2) {
        printf("Invalid number of arguments specified.\n");
	return EXIT_FAILURE;
    }

    BoatPointers boatPtrs;
    int count = 0;

    FILE *CSV;
    CSVLine line;
    int C1;
    int charCount = 0;
    if ((CSV = fopen(argv[1],"r")) != NULL) {
        while ((C1 = fgetc(CSV)) != EOF) {
            line[charCount] = (char) C1;
            charCount++;
            //Process line from CSV
	    if (C1 == '\n') {
		line[charCount-1] = '\0'; 
                addBoatFromCSVLine(boatPtrs,&count,line);
	        charCount = 0;
            }
	}
    } else {
        printf("Unable to read CSV file.\n");
	return EXIT_FAILURE;
    }

    fclose(CSV);

    //Interact with the user
    printf("\nWelcome to the Boat Management System\n");
    printf("-------------------------------------\n");

    int C2;
    BoatName boatName;
    do {
        printf("\n(I)nventory, (A)dd, (R)emove, (P)ayment, (M)onth, e(X)it : ");
        C2 = getchar();
        while (getchar() != '\n'); /*only take one 
character, clear the rest of the buffer. */

	//Handle commands
	if (tolower(C2) == 'i') { //print inventory
            printInventory(boatPtrs,count);
	} else if (tolower(C2) == 'a') { //add boat
            printf("Please enter the boat data in CSV format                 : ");
            fgets(line,sizeof(line),stdin);
            line[strlen(line)-1] = '\0'; //replace newline with end of string
            addBoatFromCSVLine(boatPtrs,&count,line);
	} else if (tolower(C2) == 'r') { //remove boat
            printf("Please enter the boat name                               : ");
            fgets(boatName,sizeof(boatName),stdin);
            boatName[strlen(boatName)-1] = '\0';
            removeBoat(boatPtrs,&count,boatName);
	} else if (tolower(C2) == 'p') { //accept a payment for the boat
            printf("Please enter the boat name                               : ");
            fgets(boatName,sizeof(boatName),stdin);
            boatName[strlen(boatName)-1] = '\0';
            acceptPayment(boatPtrs,&count,boatName);
        } else if (tolower(C2) == 'm') { //update amounts owed for a new month
            updateAmounts(boatPtrs,count);
	} else if (tolower(C2) == 'x') {
            // exit menu
	} else {
            printf("Invalid option %c\n",C2);
	}
    } while (tolower(C2) != 'x');

    printf("\nExiting the Boat Management System\n");

    //Export boat data to CSV upon exit
    int Index;
    TrailorTag placeID;
    if ((CSV = fopen(argv[1],"w")) != NULL) {
        for (Index = 0;Index < count;Index++) {
            //convert placeID to string for output
            if (boatPtrs[Index]->placeType == slip) {
                sprintf(placeID,"%d",boatPtrs[Index]->placeID.slipID);
	    } else if (boatPtrs[Index]->placeType == land) {
                placeID[0] = boatPtrs[Index]->placeID.landID;
		placeID[1] = '\0';
            } else if (boatPtrs[Index]->placeType == trailor) {
                strncpy(placeID,boatPtrs[Index]->placeID.trailorTag,sizeof(placeID));
            } else if (boatPtrs[Index]->placeType == storage) {
                sprintf(placeID,"%d",boatPtrs[Index]->placeID.storageSpace);
            }
           

            fprintf(CSV,"%s,%d,%s,%s,%.2lf\n",boatPtrs[Index]->name,
boatPtrs[Index]->length,PlaceToString(boatPtrs[Index]->placeType),placeID,
boatPtrs[Index]->moneyOwed);
        }
    } else {
        printf("Unable to write to CSV file.\n");
    }

    fclose(CSV);

    //Free memory
    for (Index = 0;Index < count;Index++) {
        free(boatPtrs[Index]);
    }

    return EXIT_SUCCESS;
}
