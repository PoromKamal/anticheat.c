#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/md5.h>
#include <dirent.h>
#include "hashUtil.h"
#define MAX_FILE_PATH 256

// This function is used to generate a hash for a file
char* hash_file(char* filename){
    //Open the file to hash
    FILE *fp = fopen(filename, "r");
    if(fp == NULL){
        perror("Could not open file\n");
        exit(1);
    }

    //Initialize MD5 digest, and context
    unsigned char md5_digest[MD5_DIGEST_LENGTH];
    MD5_CTX md5_context;
    MD5_Init(&md5_context);

    //Read the file in chunks, and update the MD5 context
    unsigned char buffer[1024];
    int bytes;
    while((bytes = fread(buffer, 1, 1024, fp)) != 0){
        MD5_Update(&md5_context, buffer, bytes);
    }
    //Finalize the MD5 hash
    MD5_Final(md5_digest, &md5_context);

    //Place our MD5_hash digest into a string, and return
    char *result = (char*)calloc(MD5_DIGEST_LENGTH*2+1, sizeof(char));
    for(int i = 0; i < MD5_DIGEST_LENGTH; i++){
        sprintf(result+i*2, "%02x", md5_digest[i]); //Lowkey kinda clever :)
    }
    return result;
}

// Hash every file in this directory, and write output to a file output
void hash_dir(char* dirName, char* outputFile){
    //Open the directory
    DIR *dir = opendir(dirName);
    if(dir == NULL){
        perror("Could not open directory\n");
        exit(1);
    }

    //Open the output file
    FILE *fp = fopen(outputFile, "w");
    if(fp == NULL){
        perror("Could not open output file\n");
        exit(1);
    }

    //Iterate through the directory
    struct dirent *entry;
    while((entry = readdir(dir)) != NULL){
        //Ignore the current directory, and parent directory
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        //Create the full path to the file
        char *fullPath = (char*)calloc(strlen(dirName)+strlen(entry->d_name)+2, sizeof(char));
        strcat(fullPath, dirName);
        strcat(fullPath, "/");
        strcat(fullPath, entry->d_name);

        //Hash the file
        char *hash = hash_file(fullPath);

        //Write the hash to the output file with the form-> filename::expectedhash
        fprintf(fp, "%s::%s\n", entry->d_name, hash);

        //Free memory
        free(fullPath);
        free(hash);
    }
    fprintf(fp, "===\n");
    //Close the directory
    closedir(dir);
    //Close the output file
    fclose(fp);
}

/*Compare the hashes in the verification file to the hashes of the files in the game directory
Return true if they match
Return false if they do not match
*/
bool compare_hashes(char* game_dir, char* verificationFile){
    //Open the verification file
    FILE *fp = fopen(verificationFile, "r");
    if(fp == NULL){
        perror("Could not open verification file\n");
        exit(1);
    }
    //Iterate through the verification file
    char *line = (char*)calloc(1024, sizeof(char));
    //Iterate through every line in the verification file, and compare hashes with fgets
    int length=0;
    while(fgets(line, 1024, fp) != NULL && strcmp(line, "===\n") != 0){
        length++;
    }
    rewind(fp);
    int failedFileNameIdx=0;
    char **failingFileNames=(char**)calloc(length,sizeof(char));
    while(fgets(line, 1024, fp) != NULL && strcmp(line, "===\n") != 0){
        //Get the filename, and expected hash from the line
        char *filename = strtok(line, "::");
        char *expectedHash = strtok(NULL, "::");
        //Remove trailing newline from expected hash
        expectedHash[strcspn(expectedHash, "\n")] = 0;

        //Create the full path to the file
        char *fullPath = (char*)calloc(strlen(game_dir)+strlen(filename)+2, sizeof(char));
        strcat(fullPath, game_dir);
        strcat(fullPath, "/");
        strcat(fullPath, filename);

        //Hash the file
        char *hash = hash_file(fullPath);
        //printf("file: %s, hash: %s, expected: %s\n", fullPath, hash, expectedHash);
        //Compare the hashes
        if(strcmp(hash, expectedHash) != 0){
            failingFileNames[failedFileNameIdx] = fullPath;
            failedFileNameIdx++;
        }
        //Free memory
        free(hash);
    }

    //If any files failed
    if(failedFileNameIdx > 0){
        char *failedOutputFileName="VerificationFailed.txt";
        FILE *failedOutputFile=fopen(failedOutputFileName, "w");
        if (failedOutputFile==NULL){
            printf("Error opening file %s", failedOutputFileName);
        }

        fprintf(failedOutputFile, "List of Failed Files:\n==========================\n");
        for (int i=0;i<failedFileNameIdx;i++){
            fprintf(failedOutputFile, "%s\n", failingFileNames[i]);
        }
        
        fclose(failedOutputFile);
    }

    for(int i = 0; i < failedFileNameIdx; i ++){
        free(failingFileNames[i]);
    }

    free(failingFileNames);
    free(line);
    //Close the verification filehow to 
    fclose(fp);

    //Return true iff all hashes match
    return failedFileNameIdx > 0 ?  false : true; 
}

//char* string = "Hisdfasdlgkhnasolg;na;dlsfngalksg"
//printf(string); "Hisdfsdvsdfgsgasdgsdg"
