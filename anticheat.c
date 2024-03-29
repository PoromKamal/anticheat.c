#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "anticheat.h"
#include "hashUtil.h"
#define VERIFICATION_FILE "verification.txt"

void appOptions(int argc, char** argv, AppOptions* appOptions){   
    if(argc < 2){
        perror("Usage:\n anticheat <game directory> -v(for verifying game files)"
                "\nanticheat <game directory> -g(For generating verification files)\n"
                "\nanticheat <game directory> -t DEV_KEY(For creating test directory, for dev purposes)\n");
        exit(1);
    }
    appOptions->argCount=argc;
    appOptions->args = (char**)calloc(argc,(sizeof(char**)));
    for (int i=0;i<argc;i++) {
        appOptions->args[i] = (char*)calloc(strlen(argv[i]),(sizeof(char)));
        strcpy(appOptions->args[i],argv[i]);
    }
    appOptions->gameDir = argv[1];
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-v") == 0){
            appOptions->mode = VERIFY;
        }
        else if(strcmp(argv[i], "-g") == 0){
            appOptions->mode = GENERATE;
        } 
        else if(strcmp(argv[i], "-igf")==0){
            appOptions->mode = IGFGENERATE;
        }
    }
    //test rebasing, hello nehal
}

void generate(AppOptions* appOptions){   
    hash_dir(appOptions, VERIFICATION_FILE);
    //Test rebasing
}

bool verify(char *gameDir){
    return compare_hashes(gameDir, VERIFICATION_FILE);
}

void start(AppOptions* appOptions){
    if(appOptions->mode == GENERATE || appOptions->mode == IGFGENERATE){
        generate(appOptions);
    }
    else if(appOptions->mode == VERIFY){
        bool verificationResult = verify(appOptions->gameDir);
        if(verificationResult){ //Temporary solution for development, use signals later
            printf("Verification successful\n");
        } else {
            printf("Verification failed\n");
        }
    } else {
        perror("Invalid mode\n");
        exit(1);
    }
}

//Creates a test directory for testing purposes, with 100 files, with 5 mb each
void create_test_dir(char* dir_name){
    char command[100];
    sprintf(command, "mkdir %s", dir_name);
    system(command);
    for(int i = 0; i < 150; i++){
        sprintf(command, "dd if=/dev/urandom of=%s/file%d bs=5M count=1", dir_name, i);
        system(command);
    }
}