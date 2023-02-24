//
//
//

#include "../include/geko.h"

int main(int argc, char *argv[]) {
    int i;

    if (argc < 2) {
        printHelp();
        exit(1);
    }

    user_prompt = getCommand(argc,argv);
    i = handleArgs(argc, argv);     // handle the command line arguments

    readConfigFile();               // read the config file

    if (silence == 0) {
        printPars();
    }

    doTask();

    return 0;
} // end main()

