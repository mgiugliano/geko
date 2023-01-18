//
//
//

#include <stdio.h>   // for printf()
#include <unistd.h>  // for getopt()
#include <stdlib.h>  // for exit()
#include <time.h>    // for time()

#include <ctype.h>   // for isprint()
//#include <NIDAQmx.h> // for NIDAQmx

// CONSTANTS
#define FILESUFFIX ".dat"  // suffix for the output data file
#define VER "0.1.0"
#define GREET "geko v" VER " - geKo electrophysiology Kommander"



// GLOBAL VARIABLES (initialized to zero)
unsigned int srate  = 0;   // sampling rate
unsigned int nAI    = 0;   // number of analog inputs channels
unsigned int nAO    = 0;   // number of analog outputs channels

char *FileName;     // name of the output data file to be created


// FUNCTION PROTOTYPES
char *getUniqueFileName(); // function to generate a unique file name
int handleArgs(int argc, char *argv[]); // function to handle the command line arguments
void checkPars(); // function to check the command line args against defaults
void printPars(); // function to print the parameters used by the program
void printHelp(); // function to print the help message


// FUNCTION DEFINITIONS

// Function to generate a unique (outut data) file name
char *getUniqueFileName() {         //------------------------------------------
// The first 8 characters are the date in the format YYYYMMDD.
// The last 9 characters are the time in the format HHMMSSmmm.
// The code is generated using the current time and date.
    time_t t = time(NULL);                    // get current time
    struct tm tm = *localtime(&t);            // convert to local time
    char *fname = malloc(21);                  // allocate memory for the code

    sprintf(fname, "%04d%02d%02d_%02d%02d%02d%s", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, FILESUFFIX);

    return fname;
} // end of getUniqueFileName() ------------------------------------------------



// Function to handle by getopts the command line arguments
int handleArgs(int argc, char *argv[]) {    //----------------------------------
    int c;                                   // variable to store the option

    while ((c = getopt(argc, argv, "s:i:o:h:")) != -1) {
        switch (c) {
            case 's':
                srate = atoi(optarg);
                break;
            case 'i':
                nAI = atoi(optarg);
                break;
            case 'o':
                nAO = atoi(optarg);
                break;
            case 'h':
                printHelp();
                exit(0);
            case '?':
                if (optopt == 's' || optopt == 'a' || optopt == 'o')
                    fprintf(stderr, "Option -%c requires an argument.", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option `-%c'.", optopt);
                else
                    fprintf(stderr, "Unknown option character `\\x%x'.", optopt);
                exit(1);
            default:

                abort();
        }   
    }
    // return the index pointint to the first non-option argument
    return optind;
} // end of handleArgs() -------------------------------------------------------



// Function to check the command line arguments against defaults
void checkPars() {                       //-------------------------------------
    if (srate <= 0) {
        srate = 15000;     // 15 kHz default sampling rate
    }
    if (nAI <= 0) {
        nAI = 1;           // 1 analog input channel by default
    }
    if (nAO <= 0) {
        nAO = 0;           // 0 analog output channel by default
    }
} // end of checkPars() --------------------------------------------------------



// Function to print the parameters used by the program
void printPars() {                       //-------------------------------------
    printf("Sampling rate: %d Hz\n", srate);
    printf("# ai: %d\n", nAI);
    printf("# ao: %d\n", nAO);
} // end of printPars() --------------------------------------------------------


// Function to print the help message
void printHelp() {                       //-------------------------------------
    printf("%s\n\n", GREET);
    printf(" Usage: geko [options] \n");   
    printf(" Options: \n");    
    printf(" -s <sampling rate> \n");
    printf(" -i <number of analog inputs> \n");
    printf(" -o <number of analog outputs> \n");
    printf(" -h print this help message \n");
} // end of printHelp() --------------------------------------------------------


// Function to launch data acquisition from channel 0 for 10 seconds
void readAI() {
    DAQmxErrChk (DAQmxCreateTask("", &taskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(taskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1000));
    DAQmxErrChk (DAQmxStartTask(taskHandle));
    DAQmxErrChk (DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000,&read,0));
    DAQmxErrChk (DAQmxStopTask(taskHandle));
    DAQmxErrChk (DAQmxClearTask(taskHandle));
    printf("Acquired %d samples \n",read);
    if( read>0 ) {
        printf("Acquired %f samples \n",data[0]);
        printf("Acquired %f samples \n",data[1]);
    }
    return 0;
}

void writeAO() {
    DAQmxErrChk(DAQmxCreateTask("",&taskHandle));
    DAQmxErrChk(DAQmxCreateAOVoltageChan(taskHandle,"Dev1/ao0","",0.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1000));
    DAQmxErrChk(DAQmxStartTask(taskHandle));
    DAQmxErrChk(DAQmxWriteAnalogF64(taskHandle,1000,0,10.0,DAQmx_Val_GroupByChannel,data,NULL,NULL));
    DAQmxErrChk(DAQmxStopTask(taskHandle));
    DAQmxErrChk(DAQmxClearTask(taskHandle));
    return 0;
}

void readAI_and_writeAO_synchronously() {
    DAQmxErrChk(DAQmxCreateTask("",&taskHandle));
    DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","",DAQmx_Val_Cfg_Default,-10.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk(DAQmxCreateAOVoltageChan(taskHandle,"Dev1/ao0","",0.0,10.0,DAQmx_Val_Volts,NULL));
    DAQmxErrChk(DAQmxCfgSampClkTiming(taskHandle,"",10000.0,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1000));
    DAQmxErrChk(DAQmxStartTask(taskHandle));
    DAQmxErrChk(DAQmxReadAnalogF64(taskHandle,1000,10.0,DAQmx_Val_GroupByChannel,data,1000,&read,0));
    DAQmxErrChk(DAQmxWriteAnalogF64(taskHandle,1000,0,10.0,DAQmx_Val_GroupByChannel,data,NULL,NULL));
    DAQmxErrChk(DAQmxStopTask(taskHandle));
    DAQmxErrChk(DAQmxClearTask(taskHandle));
    return 0;
}