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

    // Check if i is 1. If so, run function generateStim
    if (i == 1) {
        printf("Stimulation protocol provided: ");
        generateStimArray(argv[i]);

    } else {
        printf("No stimulation protocol provided. Default stimulation is 0 for 1 s.\n");
        generateStimArray("default.stim");
    }

    readwriteFinite();

    
    // FileName = getUniqueFileName();
    // printf("FileName: %s\n", FileName);

    // free(FileName);

    return 0;
} // end main()

