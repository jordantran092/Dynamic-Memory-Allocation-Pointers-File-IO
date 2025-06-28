


/*

Units:
- Handling Command Line Arguments
- User Input + Dynamic Memory Allocation
- Sorting The Input
- File Output

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>

//Min input size is 1 because 0 inputs would be useless, minimum buffer size is 3 to allow for one char to be the minimum input accounting for null char and \n from fgets.
#define MIN_INPUT_SIZE 1
#define MIN_BUFFER_SIZE 3

bool isNum(char str[]);
int compare(const void* x, const void* y);
void printArray(char** array, int n);
int isEndHaveSpaceWithDigits(char* userInput);
char* substr(char* str, int startIndex, int endIndex);



int main(int argc, char *argv[]) {
 

    //checking for valid inputs. first checking for some argument in num of inputs and buffer size as the bare minimum. once that's confirmed, it'll check if those arguments are numbers not words, and checks if the values are legal values based on the pre-processing defined values
    if(argc >= 3 && 
        (isNum(argv[1]) && strtoul(argv[1], NULL, 0) >= MIN_INPUT_SIZE) && 
        (isNum(argv[2]) && strtoul(argv[2], NULL, 0) >= MIN_BUFFER_SIZE)) { 

            

        int numOfInputs = strtoul(argv[1], NULL, 0);
        int bufferSize = strtoul(argv[2], NULL, 0);
        char *fileName = argv[3];

        //the array of strings will be created, where strList is an array of pointers to pointers to chars. If memory allocation fails meaning strList is returned with NULL, then will print error message and terminate the program with 1;
        char **strList = calloc(numOfInputs, sizeof(char*));
        if(strList == NULL) {
            printf("Memory allocation failed for strList\n");
            return 1;
        }

        for(int i = 0; i < numOfInputs; i++) {
            strList[i] = calloc(bufferSize, sizeof(char));
            //If memory allocation fails meaning strList[i] is returned with NULL, then will print error message and terminate the program with 1;
            if(strList[i] == NULL) {
                printf("Memory allocation failed for an element of strList\n");
                return 1;
            }
        }
        
        //Get user input
        char *userInput = calloc(bufferSize, sizeof(char));
        //If memory allocation fails meaning userInput is returned with NULL, then will print error message and terminate the program with 1;
        if(userInput == NULL) {
            printf("Memory allocation failed for user input\n");
            return 1;
        }
        printf("Enter %d lines (each ending with a positive number): \n", numOfInputs);
        for(int i = 0; i < numOfInputs; i++) {
            fgets(userInput, bufferSize, stdin);
            
            userInput[strlen(userInput)-1] = '\0';


            //checks if user input is 1 len str, if so will append to array. will also append if the string is legal in terms of having a space before the trailing digit-only consecutive chars at the end of string
            if((strlen(userInput) == 1 && isdigit(userInput[0])) || isEndHaveSpaceWithDigits(userInput)) { 
                strcpy(strList[i], userInput);
            }
            else {
                //if string is not legal, then will output error and clean up the pointers, free the memory that was allocated for the rest of the program because no longer useful, then the program will terminate
                printf("\nError: No number with a space before, at the end of the input string '%s'\n", userInput);
                free(userInput);
                userInput = NULL; 

                for(int i = 0; i < numOfInputs; i++) {
                    free(strList[i]);
                }
                free(strList);
                strList = NULL;
        
                return EXIT_FAILURE;
            }

        }   


        //array of strings will be sorted according to their numbering at the end of the string
        qsort(strList, numOfInputs, sizeof(char**), compare); 



        //if no output file was inputted, will print the sorted array in stdout
        if(argc == 3) {
            printf("\n\nSorted output based on their ranking in non-descending order:");
            printArray(strList, numOfInputs);
        }
        else {
            //this will save the output into the desired output file, if not exist will create, else will overwrite it
            FILE* file;
            file = fopen(argv[3], "w");
 

            for(int i = 0; i < numOfInputs; i++) {
                fprintf(file, strList[i]);
                fprintf(file, "\n");
            }

            fclose(file);
            
            
            printf("\nSorted output saved to %s\n", argv[3]);
        }
            
    
    }
    else {
        //if buffer size was not valid, will print out error and terminate
        if(isNum(argv[2]) && !(strtoul(argv[2], NULL, 0) >= MIN_BUFFER_SIZE)) {
            printf("Invalid input! Buffer size must be at least %d which corresponds to one char\n", MIN_BUFFER_SIZE);
        }
        else 
            //if input was invalid will print out error and terminate
            printf("Invalid input! Usage: ./dmac <number of inputs > <buffer size > <output file (optional)>\n\n");
    }
    

}

bool isNum(char str[]) {
    //goes through each char of string and checks if whole string is a number
    bool isNum = true;
    for(int i = 0; isNum && i < strlen(str); i++) {
        isNum = isdigit(str[i]);
    }
    return isNum;
}

int isEndHaveSpaceWithDigits(char* userInput) {
    int isEndHaveSpaceWithDigits = 0;
    //first checks if last index of string is a digit. if not, then automatically know the string doesn't have a numbering at the end
    if(isdigit(userInput[strlen(userInput)-1])) {

        //otherwise, will continue checking rest of string until reach a non-digit. once reach a non digit, make sure it's a space. if not a space then will know string doesn't have a numbering at the end with a space.

        //but if does have space, then will check if that is the last space in the string to make sure it has a proper numbering with space at end
        int index = strlen(userInput)-2;
        while(isNum(&userInput[index])) {
            index--;
        }
        if(userInput[index] == ' ' && (strrchr(userInput, ' ') == &userInput[index])) {
            isEndHaveSpaceWithDigits = 1;
        }
        //otherwise will still be 0 because invalid input at this point
        
    }
    return isEndHaveSpaceWithDigits;
}

int extractNum(const char *str1) {
    int startIndex = 0;
    int endIndex = 0;
    
    //gets the starting index and end index of the numbering at end of string, then will extract that substring and convert it into a number and return that

    startIndex = strrchr(str1, ' ')+1 - str1;
    endIndex = strlen(str1)-1;
    char* str1Num = substr(str1, startIndex, endIndex);
    int num1 = strtoul(str1Num, NULL, 0);
    free(str1Num);


    return num1;
}



int compare(const void* x, const void* y) { //actual argument is a **x because we're rearranging the double pointers/strings in the strList level, not the re-arranging the chars of each string. Then, the argument is casted into a *x
    
    //have to dereference x to get the starting address of the string because it's working with **x in the strList level, whereas if you did x[some k], it is already dereferenced by giving the address of the pointer/string with x[k]
    const char *str1 = *(const char**) x;
    const char *str2 = *(const char**) y;

    //For qsort, will extract the end numbers from the given arguments of strings, and compare them. > 0 means x is after y. < 0 means x is before y,  == 0 is x and y are same. 
    int num1 = extractNum(str1);
    int num2 = extractNum(str2);

    return num1-num2;


}

void printArray(char** array, int n) {

    for(int i = 0; i < n; i++) {
        printf("\n%s", array[i]);
    }
    printf("\n");
}

char* substr(char* str, int startIndex, int endIndex) {
    //will go through each char in the given string between the given indexes, and append it into a new string and return that
    char *result = calloc(endIndex-startIndex+1, sizeof(char));
    //If memory allocation fails meaning calloc is returned with NULL, then will print error message and terminate the program;
    if(result == NULL) {
        printf("Memory allocation failed for substr method\n");
        exit(EXIT_FAILURE);
    }


    for(int i = startIndex; i < endIndex+1; i++) {
        strncat(result, &str[i], 1);
    }
    return result;
}
