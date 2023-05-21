#include "anticheat.h"
#ifndef HASH_UTIL
#define HASH_UTIL

typedef struct file_hash{
    char* filename;
    char* hash;
}file_hash;
char* hash_file(char* filename);
void hash_dir(AppOptions* appOptions, char* outputFile);
bool compare_hashes(char* verificationFile, char* game_dir);
#endif