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

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

/*********************************************/
// DAQmx Configuration Options
/*********************************************/

float64 srate = 0; // The sampling rate in samples per second per channel.

char *aiCh = NULL;
char *aoCh = NULL;
float64 minVal = -10.0; // The minimum value, in units, that you expect to generate.
float64 maxVal = 10.0; // The maximum value, in units, that you expect to generate.

float64 timeout = 10; // The amount of time, in seconds, to wait for the function to read the sample(s).

// TODO: NOT RESPONDING
bool32 dataLayout = DAQmx_Val_GroupByScanNumber; // Specifies how the samples are arranged, either interleaved or noninterleaved. Options: DAQmx_Val_GroupByChannel, DAQmx_Val_GroupByScanNumber

// GLOBAL VARIABLES (initialized to zero)
unsigned int nA    = 0;   // number of analog inputs channels
char *stimFile = NULL;    // name of the stimulation protocol file
char *FileName;     // name of the output data file to be created

//float64 data[];
float64 stimArray[50];


// FUNCTION PROTOTYPES
char *getUniqueFileName(); // function to generate a unique file name
int handleArgs(int argc, char *argv[]); // function to handle the command line arguments
void checkPars(); // function to check the command line args against defaults
void printPars(); // function to print the parameters used by the program
void printHelp(); // function to print the help message
void readwriteFinite(); 
void createDataStore();


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

    while ((c = getopt(argc, argv, "s:c:h:o:")) != -1) {
        switch (c) {
            case 's':
                srate = atoi(optarg);
                break;
            case 'c':
                nA = atoi(optarg);
                break;
            case 'h':
                printHelp();
                exit(0);
            case 'o':
                stimFile = optarg;
                break;
            case '?':
                if (optopt == 's' || optopt == 'i' || optopt == 'o')
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
        srate = 10;     // 15 kHz default sampling rate
    }
    if (nA <= 1) {
        nA = 1;                 // 1 analog input channel by default
        aiCh = "Dev1/ai0:1";
        aoCh = "Dev1/ao0";
    } else if (nA >= 2) {
        nA = 2;
        aiCh = "Dev1/ai0:3";
        aoCh = "Dev1/ao0:1";
    }
    if (stimFile == NULL) {
        stimFile = "default.stim";
    }
    
} // end of checkPars() --------------------------------------------------------



// Function to print the parameters used by the program, separated by ========
void printPars() {                       //-------------------------------------
    printf("\n");
    printf("========================================\n");
    printf("\n");
    printf("Sampling rate: %g Hz\n", srate);
    printf("Number of active channels: %d\n", nA);
    printf("Stimulation protocol file: %s\n", stimFile);
    printf("\n");
    printf("========================================\n");
    printf("\n");
} // end of printPars() --------------------------------------------------------


// Function to print the help message
void printHelp() {                       //-------------------------------------
    printf("%s\n\n", GREET);
    printf(" Usage: geko [options] \n");   
    printf(" Options: \n");    
    printf(" -s <sampling rate> \n");
    printf(" -c <number of analog channels> \n");
    printf(" -o <stimulation protocol filename> \n");
    printf(" -h print this help message \n");
} // end of printHelp() --------------------------------------------------------


// Function to generate the stimulation array, based on the filename supplied
void generateStimArray() {      //-----------------------------------------------
    // stimFile
    // stimArray = stimgen(stim);

    // Assign to the variable stimArray the value of zero vector
    // of length 1 s * sampling rate
    printf("Generating stimulation array...\n");
    int i;
    for (i = 0; i < 50; i++)
    {
        stimArray[i] = i/10;
    }    
    int32 size_stim = sizeof(stimArray)/sizeof(stimArray[0]);
    printf("Stimulation array size: %d\n", size_stim);
    return;

} // end of generateStimArray() -------------------------------------------------


// Generalized function to interact with DAQ
void readwriteFinite() {
    // Variables associated with the NIDAQ tasks
    TaskHandle AITaskHandle=0, AOTaskHandle=0;
    int32 read; // How many samples have been read
    int32 written; // How many samples have been written
    int32 sampsPerChan = sizeof(stimArray)/sizeof(stimArray[0]); // Number of samples to generate/acquire per channel.
    int32 size_data = sampsPerChan*nA*2; // Size of the data array
    float64 data[size_data]; // Data array to be written

    FILE *fp;
    int error=0;
    char errBuff[2048]={'\0'};

    FileName = getUniqueFileName();
    DAQmxErrChk (DAQmxCreateTask("",&AITaskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(AITaskHandle,aiCh,"",DAQmx_Val_RSE,minVal,maxVal,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(AITaskHandle,NULL,srate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,sampsPerChan));
  
    DAQmxErrChk (DAQmxCreateTask("",&AOTaskHandle));   
    DAQmxErrChk (DAQmxCreateAOVoltageChan(AOTaskHandle,aoCh,"",minVal,maxVal,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(AOTaskHandle,"",srate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,sampsPerChan));

    // Connect AO start to AI start
    DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(AOTaskHandle, "ai/StartTrigger", DAQmx_Val_Rising));

    // Arm the AO task
    DAQmxWriteAnalogF64(AOTaskHandle,sampsPerChan,FALSE,timeout,dataLayout, stimArray, &written, NULL);

    // Start the AI task
    printf("Performing the task...\n");
    DAQmxErrChk (DAQmxStartTask(AOTaskHandle));
    DAQmxErrChk (DAQmxStartTask(AITaskHandle));

    DAQmxErrChk (DAQmxReadAnalogF64(AITaskHandle,sampsPerChan,timeout,dataLayout,data,size_data,&read,NULL));
    // int32 DAQmxReadAnalogF64 (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, float64 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);

    DAQmxErrChk (DAQmxClearTask(AOTaskHandle));
    DAQmxErrChk (DAQmxClearTask(AITaskHandle));
        
    //fp = fopen(FileName, "wb");
    //fwrite(&data, sizeof(data), 1, fp);
    //fclose(fp);

    FILE *fp2;
    fp2 = fopen("dataStore.txt", "w");
    fprintf(fp2, "%-20s\t%-20s\t%-20s\n", "ai0", "ai1", "ao0");
    int i;
    for (int i = 0; i < sampsPerChan; i++)
        fprintf(fp2, "%-20.4f\t%-20.4f\t%-20.4f\n", data[2*i], data[2*i+1],stimArray[i]);
    
    fclose(fp2);

    printf("Task completed.\n");

    Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( AITaskHandle ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(AITaskHandle);
		DAQmxClearTask(AITaskHandle);
		AITaskHandle = 0;
	}
	if( AOTaskHandle ) {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(AOTaskHandle);
		DAQmxClearTask(AOTaskHandle);
		AOTaskHandle = 0;
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);

    return;
}

void createDataStore() {
  
    return;


}



