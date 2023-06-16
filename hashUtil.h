#ifndef HASH_UTIL
#define HASH_UTIL
typedef struct file_hash{
    char* filename;
    char* hash;
}file_hash;

typedef struct hash_func_args{
    int limit; //The number of files to hash
    struct file_hash* fileHashes; //Array of file hashes
    char* game_dir;
}hash_func_args;

typedef struct hash_func_result{
    char** failedFileNames;
    int failedFileNamesLen;
}hash_func_result;

char* hash_file(char* filename);
void hash_dir(char* dirName, char* outputFile);
bool compare_hashes(char* verificationFile, char* game_dir);
#endif