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
                "\nanticheat <game directory> -g(For generating verification files)\n");
        exit(1);
    }
    appOptions->gameDir = argv[1];
    for(int i = 0; i < argc; i++){
        if(strcmp(argv[i], "-v") == 0){
            appOptions->mode = VERIFY;
        }
        else if(strcmp(argv[i], "-g") == 0){
            appOptions->mode = GENERATE;
        }
    }
}

void generate(char *gameDir){
    hash_dir(gameDir, VERIFICATION_FILE);
    //Test rebasing
}

bool verify(char *gameDir){
    return compare_hashes(gameDir, VERIFICATION_FILE);
}

void start(AppOptions* appOptions){
    if(appOptions->mode == GENERATE){
        generate(appOptions->gameDir);
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