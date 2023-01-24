//
//
//

#include <stdio.h>   // for printf()
#include <unistd.h>  // for getopt()
#include <stdlib.h>  // for exit()
#include <time.h>    // for time()

#include <ctype.h>   // for isprint()
#include <NIDAQmx.h> // for NIDAQmx

// CONSTANTS
#define FILESUFFIX ".dat"  // suffix for the output data file
#define VER "0.1.0"
#define GREET "geko v" VER " - geKo electrophysiology Kommander"

/*********************************************/
// DAQmx Configuration Options
/*********************************************/

int srate = 0; // The sampling rate in samples per second per channel.
int32 sampsPerChan = 1000; // Number of samples to generate/acquire per channel.

float64 minVal = -10.0; // The minimum value, in units, that you expect to generate.
float64 maxVal = 10.0; // The maximum value, in units, that you expect to generate.

float64 timeout = 10; // The amount of time, in seconds, to wait for the function to read the sample(s).
bool32 dataLayout = DAQmx_Val_GroupByChannel; // Specifies how the samples are arranged, either interleaved or noninterleaved. Options: DAQmx_Val_GroupByChannel, DAQmx_Val_GroupByScanNumber

// GLOBAL VARIABLES (initialized to zero)
unsigned int nAI    = 0;   // number of analog inputs channels
unsigned int nAO    = 0;   // number of analog outputs channels

char *FileName;     // name of the output data file to be created


// FUNCTION PROTOTYPES
char *getUniqueFileName(); // function to generate a unique file name
int handleArgs(int argc, char *argv[]); // function to handle the command line arguments
void checkPars(); // function to check the command line args against defaults
void printPars(); // function to print the parameters used by the program
void printHelp(); // function to print the help message
void readwriteFinite(); 


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


// Generalized function to interact with DAQ
void readwriteFinite() {
    // Variables associated with the NIDAQ tasks
    TaskHandle AITaskHandle=0, AOTaskHandle=0;
    int32 read; // How many samples have been read
    int32 written; // How many samples have been written
    float64 data[1000];
    float64 stim[1000];
    char *aiCh;
    char *aoCh;
    FILE *fp;

    // Check how many channels are active
    if (nAI == 1) {
        aiCh = "Dev1/ai0:1";
    }
    else {
        aiCh = "Dev1/ai0:3";
    }
    printf("AI: %s\n",aiCh);
    if (nAO == 1) {
        aoCh = "Dev1/ao0";
    }
    else {
        aoCh = "Dev1/ao0:1";
    }
    printf("AO: %s\n",aoCh);

    DAQmxCreateTask("",&AITaskHandle);
    DAQmxCreateAIVoltageChan(AITaskHandle,aiCh,"",DAQmx_Val_RSE,minVal,maxVal,DAQmx_Val_Volts,NULL);
    DAQmxCfgSampClkTiming(AITaskHandle,"",srate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1000);
  
    DAQmxCreateTask("",&AOTaskHandle);   
    DAQmxCreateAOVoltageChan(AOTaskHandle,aoCh,"",minVal,maxVal,DAQmx_Val_Volts,NULL);
    DAQmxCfgSampClkTiming(AOTaskHandle,"",srate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,1000);

    // Connect AO start to AI start
    DAQmxCfgDigEdgeStartTrig(AOTaskHandle, "ai/StartTrigger", DAQmx_Val_Rising);

    // HERE WE NEED TO GENERATE THE OUTPUT DATA
    int j;
    for (j=0; j<1000; j++)
        stim[j] = 5.0;


    // Arm the AO task
    DAQmxWriteAnalogF64(AOTaskHandle,sampsPerChan,FALSE,timeout,dataLayout, stim, &written, NULL);
    // int32 DAQmxWriteAnalogF64 (TaskHandle taskHandle, int32 numSampsPerChan, 
    //          bool32 autoStart, float64 timeout, bool32 dataLayout, float64 writeArray[], 
    //          int32 *sampsPerChanWritten, bool32 *reserved);

    // Start the AI task

    DAQmxReadAnalogF64(AITaskHandle,sampsPerChan,10.0,dataLayout,data,sampsPerChan,&read,NULL);
    // int32 DAQmxReadAnalogF64 (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, float64 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);


    // DAQmxStopTask(analogOutputHandle);
    // DAQmxStopTask(analogInputHandle);

    DAQmxClearTask(AOTaskHandle);
    DAQmxClearTask(AITaskHandle);


    fp = fopen("DATA.txt", "w");

    int i;
    for (i=0; i<2000; i++)
        fprintf(fp, "%g\n", data[i]);

    fclose(fp);
}

// Function to launch data acquisition from channel 0 for 10 seconds

