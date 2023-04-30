#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <openssl/md5.h>
#include <dirent.h>
#include "hashUtil.h"

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
    //Close the directory
    closedir(dir);
    //Close the output file
    fclose(fp);
}

/*Compare the hashes in the verification file to the hashes of the files in the game directory
Return true if they match
Return false if they do not match
*/
bool compare_hashes(char* verificationFile, char* game_dir){
}
