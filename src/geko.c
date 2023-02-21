//
//
//

#include "../include/geko.h"

int main(int argc, char *argv[]) {
    // Store the command used to run this program in a string
    // for later use in the output file.
    // char *command = getCommand(argc, argv);

    int i;

    if (argc < 2) {
        printHelp();
        exit(1);
    }
    user_prompt = getCommand(argc,argv);
    i = handleArgs(argc, argv);     // handle the command line arguments

    
    readConfigFile();               // read the config file
  
    checkPars();                // check the command line arguments

    printPars();

    generateStimArray();

    readwriteFinite();

    createDataStore();

    
    // FileName = getUniqueFileName();
    // printf("FileName: %s\n", FileName);

    // free(FileName);

    return 0;
} // end main()

