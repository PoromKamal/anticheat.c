#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/md5.h>
#include <pthread.h>
#include <dirent.h>
#include "hashUtil.h"
#define MAX_FILE_PATH 256
#define THREAD_DIVISOR 25

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

void *compare_hashes_helper(void* args){
    hash_func_args* func_args = (hash_func_args*)args;
    file_hash *fileHashes = func_args->fileHashes;
    int limit = func_args->limit;
    char** failingFileNames = (char**)calloc(limit, sizeof(char*));
    char* game_dir = func_args->game_dir;
    int failingFileNamesLen = 0;
    for(int i = 0; i < limit; i++){
        char* fileName = fileHashes[i].filename;
        char* expectedHash = fileHashes[i].hash;

        //Create the full path to the file
        char *fullPath = (char*)calloc(strlen(game_dir)+strlen(fileName)+2, sizeof(char));
        strcat(fullPath, game_dir);
        strcat(fullPath, "/");
        strcat(fullPath, fileName);

        //Hash the file
        char *hash = hash_file(fullPath);
        if(strcmp(hash, expectedHash) != 0){
            failingFileNames[failingFileNamesLen] = fullPath;
            failingFileNamesLen++;
        }else{
            free(fullPath);
        }
        free(hash);
    }
    hash_func_result* res = (hash_func_result*)calloc(1, sizeof(hash_func_result));
    res->failedFileNames = failingFileNames;
    res->failedFileNamesLen = failingFileNamesLen;
    free(func_args->game_dir);
    //free(func_args);
    pthread_exit((void*)res);
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

void print_result(hash_func_result* res){
    printf("Failed Files:\n");
    for(int i = 0; i < res->failedFileNamesLen; i++){
        printf("%s\n", res->failedFileNames[i]);
    }
    printf("Number of failed files: %d\n", res->failedFileNamesLen);
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
    int length=0;
    while(fgets(line, 1024, fp) != NULL && strcmp(line, "===\n") != 0){
        length++;
    }
    //Reposition file pointer back to top of the file
    rewind(fp);

    //Calculate the number of threads to create (cieling of length/50)
    int numThreads = length/50;

    //Create an array of file_hash structs
    file_hash *fileHashes = (file_hash*)calloc(length, sizeof(file_hash));

    int fileHashesIdx = 0;
    while(fgets(line, 1024, fp) != NULL && strcmp(line, "===\n") != 0){
        file_hash *fileHash = (file_hash*)calloc(1, sizeof(file_hash));
        //Get the filename, and expected hash from the line
        char *filename = strtok(line, "::");
        char *expectedHash = strtok(NULL, "::");

        //Remove trailing newline from expected hash
        expectedHash[strcspn(expectedHash, "\n")] = '\0';
        //Calloc memory for fileHash strings
        fileHash->filename = (char*)calloc(strlen(filename)+1, sizeof(char));
        fileHash->hash = (char*)calloc(strlen(expectedHash)+1, sizeof(char));
        strcpy(fileHash->filename, filename);
        strcpy(fileHash->hash, expectedHash);
        fileHashes[fileHashesIdx] = *fileHash;
        fileHashesIdx++;
    }

    int failedFileNameIdx=0;
    char **failingFileNames=(char**)calloc(length,sizeof(char));

    //Create an array of threads
    pthread_t *threads = (pthread_t*)calloc(numThreads, sizeof(pthread_t));
    //iterate through fileHashes array

    for(int i = 0; i < length; i ++){
        //Create a thread for every 50 files
        if(i % THREAD_DIVISOR == 0){
            //Prepare arguments for the thread function
            hash_func_args *thread_args = (hash_func_args*)calloc(1, sizeof(hash_func_args));
            thread_args->fileHashes = fileHashes + i; //File hashes to compare offset by i
            thread_args->limit = THREAD_DIVISOR % length;
            thread_args->game_dir = (char*)calloc(strlen(game_dir), sizeof(char));
            strcpy(thread_args->game_dir, game_dir);
            //Create a thread
            pthread_create(&threads[i / THREAD_DIVISOR], NULL, compare_hashes_helper, (void*)thread_args);
        }
    }

    //Join all threads
    for(int i = 0; i < numThreads; i ++){
        hash_func_result *res;
        pthread_join(threads[i], (void**)&res);
        //Check if any files failed
        if(res->failedFileNamesLen > 0){
            for(int j = 0; j < res->failedFileNamesLen; j ++){
                failingFileNames[failedFileNameIdx] = res->failedFileNames[j];
                failedFileNameIdx++;
            }
        }
        //Free res
        free(res);
    }


    //If any files failed
    if(failedFileNameIdx > 0){
        char *failedOutputFileName="VerificationFailed.txt";
        FILE *failedOutputFile=fopen(failedOutputFileName, "w");
        if (failedOutputFile==NULL){
            printf("Error opening file %s", failedOutputFileName);
    
        }
        else
        {
            fprintf(failedOutputFile, "List of Failed Files:\n==========================\n");
            for (int i=0;i<failedFileNameIdx;i++){
                fprintf(failedOutputFile, "%s\n", failingFileNames[i]);
            }
            fclose(failedOutputFile);
        }
        
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
