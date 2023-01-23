//
//
//

#include "../include/geko.h"
#include <stdio.h>
#include "../include/NIDAQmx.h"

int main(int argc, char **argv) {
    int i;

    if (argc < 2) {
        printHelp();
        exit(1);
    }

    i = handleArgs(argc, argv);     // handle the command line arguments
    checkPars();                // check the command line arguments

    printPars();

    for (int k = i; k < argc; k++)
        printf("%s ", argv[k]);

    printf("\n");
    
    // printf("Setup your experiment:\n");

    readwriteFinite();





    // FileName = getUniqueFileName();
    // printf("FileName: %s\n", FileName);

    // free(FileName);

    return 0;
} // end main()

