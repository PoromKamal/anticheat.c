#ifndef ANTICHEAT
#define ANTICHEAT
//Struct to decide how to run the app
#define GENERATE 0
#define VERIFY 1
#define IGFGENERATE 3
typedef struct AppOptions{
    char* gameDir;
    int argCount;
    char **args;
    int mode; //0 - generateVerifictaionFile, 1 - checkVerificationFile
}AppOptions;
//Create a AppOptions struct with arguments passed by the user
void appOptions(int argc, char** argv, AppOptions* appOptions);
void start(AppOptions* appOptions); 
#endif