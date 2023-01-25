//
//
//

#include "../include/geko.h"

int main(int argc, char **argv) {
    int i;

    if (argc < 2) {
        printHelp();
        exit(1);
    }

    i = handleArgs(argc, argv);     // handle the command line arguments
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

