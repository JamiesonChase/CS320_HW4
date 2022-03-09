/*
* Antonio Maniscalco, Chase Jamieson, Josh Sedig
* CS 460: Project 2
* Instructor: Dr. Xuechen Zhang
* Arguement parsing file
*/
#include "HEADERS.h"

// checks the inputted command line arguments and parses necessary info
int checkArgs(int argc, char *argv[]) {
    //If count is off just return and program will end
    if(argc > 7 || argc < 5) return(0);

    arg.quant = 0;
    
//check our args if they are wrong just exit program
    if(strcmp(argv[1], "-alg") == 0) {
        // check if algo name exists if so copy to our structure and if it is RR set our flag
        if (strcmp(argv[2],"SJF") == 0 || strcmp(argv[2],"FIFO") == 0 || strcmp(argv[2],"PR") == 0 || strcmp(argv[2], "RR") == 0) {
                strcpy(arg.algo, argv[2]);
                arg.algo[strlen(argv[2])] = '\0';
                if (strcmp(argv[2], "RR") == 0) isRR = F;
        } else return(0);
    }

    for(int i = 2; i < argc; i++) {
        
        if(strcmp(argv[i], "-quantum") == 0) {
            errno = 0;
            //make string to int check for valid input of quantum
            char* temp = argv[i+2]; 
            arg.quant = strtol(argv[i+1], &temp, 10); 
            if(errno == ERANGE || errno == EINVAL || argv[i+1] == temp || arg.quant <=0) return(0);
        }
        else if(strcmp(argv[i], "-input") == 0) {
            // search for file name, and attempt to open to read
            arg.fp = fopen(argv[i+1], "r");
            if(arg.fp == NULL) {
                fprintf(stderr, "%s \n", strerror(errno));
                return(0);
            } else {
                strcpy(arg.file_name, argv[i+1]);
                arg.file_name[strlen(argv[i+1])] = '\0';
                //strcat(arg.file_name, &end);
            }
        }
    }
    if(strcmp(arg.algo, "RR") == 0 && arg.quant <= 0) return(0);
    
    return (1);
}
