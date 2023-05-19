#ifndef HASH_UTIL
#define HASH_UTIL
typedef struct file_hash{
    char* filename;
    char* hash;
}file_hash;
char* hash_file(char* filename);
void hash_dir(int argc, char** argv, char* dirName, char* outputFile);
void hash_dir2(int argc, char** argv, char* dirName, char* outputFile);
bool compare_hashes(char* verificationFile, char* game_dir);
#endif