#include <stdio.h>
#include <stdlib.h>
#include "anticheat.h"
int main(int argc, char *argv[]){
    AppOptions options;
    appOptions(argc, argv, &options);
    start(&options);
    exit(0);
}