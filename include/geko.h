//
//
//

#include <stdio.h>   // for printf()
#include <unistd.h>  // for getopt()
#include <stdlib.h>  // for exit()
#include <time.h>    // for time()
#include <string.h>  // for strlen()
#include <ctype.h>   // for isprint()
#include <NIDAQmx.h> // for NIDAQmx
#include <sys/stat.h> // for mkdir()
#include <sys/types.h> // for mkdir()
#include "ini.h"     // for INI parsing
#include "ini.c"     // for INI parsing

// CONSTANTS
#define FILESUFFIX ".dat"  // suffix for the output data file
#define VER "0.1.0"
#define GREET "geko v" VER " - geKo electrophysiology Kommander"
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

/*********************************************/
// DAQmx Configuration Options
/*********************************************/

float64 minVal = -10.0; // The minimum value, in units, that you expect to generate.
float64 maxVal = 10.0; // The maximum value, in units, that you expect to generate.

float64 timeout = -1; // The amount of time, in seconds, to wait for the function to read the sample(s).

// TODO: NOT RESPONDING
bool32 dataLayout = DAQmx_Val_GroupByScanNumber; // Specifies how the samples are arranged, either interleaved or noninterleaved. Options: DAQmx_Val_GroupByChannel, DAQmx_Val_GroupByScanNumber

// GLOBAL VARIABLES (initialized to zero)
unsigned int reps = 1;   // number of repetitions of the stimulation protocol
unsigned int isi = 0;   // inter-stimulus interval in seconds
char *suffix = "";    // suffix for the output data file

char* configFile = "../geko.ini";  // name of the configuration file
char *stimFile = "default.stim";    // name of the stimulation protocol file
char * FileName = NULL;     // name of the output data file to be created
char *user_prompt = NULL;   // command used to run this program
int ai_factor = 1;
double ao_factor = 1.0;

//float64 data[];

// INI parsing
typedef struct
{
    int srate;
    int nA;
    const char* device;
    const char* aiCh;
    const char* aoCh;
    const char* mode;
    int ai_factor_cc;
    int ai_factor_vc;
    double ao_factor_cc;
    double ao_factor_vc;
} configuration;


configuration config;

// FUNCTION PROTOTYPES
char *getUniqueFileName(); // function to generate a unique file name
int handleArgs(int argc, char *argv[]); // function to handle the command line arguments
void checkPars(); // function to check the command line args against defaults
void printPars(); // function to print the parameters used by the program
void printHelp(); // function to print the help message
void readwriteFinite(); 
void createDataStore();
void print_image(FILE *fptr);
static int handler(void* user, const char* section, const char* name,
                   const char* value);
int readConfigFile();

// FUNCTION DEFINITIONS

// Function to generate a unique (outut data) file name
char *getUniqueFileName() {         //------------------------------------------
// The first 8 characters are the date in the format YYYYMMDD.
// The last 9 characters are the time in the format HHMMSSmmm.
// The code is generated using the current time and date.
    time_t t = time(NULL);                    // get current time
    struct tm tm = *localtime(&t);            // convert to local time
    char *fname = malloc(21);                  // allocate memory for the code

    sprintf(fname,"%04d%02d%02d_%02d%02d%02d%s", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, suffix);

    return fname;
} // end of getUniqueFileName() ------------------------------------------------

char *getCommand(int argc, char *argv[]) {         //------------------------------------------
int string_length;
    int i;
for (i = 0; i < argc; i++) {
        string_length = string_length + strlen(argv[i]);
    }
    char *out = malloc(string_length + 1);

    for (i = 0; i < argc; i++) {
        strcat(out, argv[i]);
        strcat(out, " ");
    }
    return out;
}
// Function to handle by getopts the command line arguments
int handleArgs(int argc, char *argv[]) {    //----------------------------------
    int c;                                   // variable to store the option

    while ((c = getopt(argc, argv, "c:s:h:r:i:x:")) != -1) {
        switch (c) {
            case 'c':
                configFile = optarg;
                break;
            case 's':
                stimFile = optarg;
                break;
            case 'h':
                printHelp();
                exit(0);
            case 'r':
                reps = atoi(optarg);
                break;
            case 'i':
                isi = atoi(optarg);
                break;
            case 'x':
                suffix = optarg;
                break;
            case '?':
                if (optopt == 'r' || optopt == 'i' || optopt == 's')
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

static int handler(void* user, const char* section, const char* name,
                   const char* value)
{
    configuration* pconfig = (configuration*)user;
    char *eptr;
    const char *devName;
    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("hardware", "device")) {
        pconfig->device = value;
        devName = value;
    } else if (MATCH("hardware", "aichans")) {
        pconfig->aiCh = value;
    } else if (MATCH("hardware", "aochans")) {
        pconfig->aoCh = value;
    } else if (MATCH("hardware", "srate")) {
        pconfig->srate = atoi(value);
    } else if (MATCH("amplifier", "mode")) {
        pconfig->mode = value;
    } else if (MATCH("amplifier", "ai_factor_cc")) {
        pconfig->ai_factor_cc = atoi(value);
    } else if (MATCH("amplifier", "ai_factor_vc")) {
        pconfig->ai_factor_cc = atoi(value);
    } else if (MATCH("amplifier", "ao_factor_cc")) {
        pconfig->ai_factor_cc = strtod(value, &eptr);
    } else if (MATCH("amplifier", "ao_factor_vc")) {
        pconfig->ai_factor_cc = strtod(value, &eptr);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

// Read the configuration file
int readConfigFile() {                 //-------------------------------------

    if (ini_parse(configFile, handler, &config) < 0) {
        printf("Can't load '%s'\n", configFile);
        return 1;
    }

    return 0;
} // end of readConfigFile() --------------------------------------------------


// Function to check the command line arguments against defaults
void checkPars() {                       //-------------------------------------
    if (config.srate <= 0) {
        config.srate = 15000;     // 15 kHz default sampling rate
    }
    if (config.nA <= 1) {
        config.nA = 1;                 // 1 analog input channel by default
        config.aiCh = "Dev1/ai0:1";
        config.aoCh = "Dev1/ao0";
    } else if (config.nA >= 2) {
        config.nA = 2;
        config.aiCh = "Dev1/ai0:3";
        config.aoCh = "Dev1/ao0:1";
    }
    if (stimFile == NULL) {
        stimFile = "default.stim";
    }
    if (config.mode == "cc") {
        ai_factor = config.ai_factor_cc;
        ao_factor = config.ao_factor_cc;
    } else if (config.mode == "vc") {
        ai_factor = config.ai_factor_vc;
        ao_factor = config.ao_factor_vc;
    }
    
} // end of checkPars() --------------------------------------------------------


FILE *fptr = NULL;
// Function to print the parameters used by the program, separated by ========
void printPars() {                       //-------------------------------------
    printf("\n");
    fptr = fopen("../img/logo.txt", "r");
    print_image(fptr);
    fclose(fptr);
    printf("\n");
    printf("========================================\n");
    printf("\n");
    printf("Sampling rate: %d Hz\n", config.srate);
    printf("Number of active channels: %d\n", config.nA);
    printf("Stimulation protocol file: %s\n", stimFile);
    printf("\n");
    printf("========================================\n");
    printf("\n");
} // end of printPars() --------------------------------------------------------


void print_image(FILE *fptr)
{
    char read_string[128];

    while(fgets(read_string,sizeof(read_string),fptr) != NULL)
        printf("%s",read_string);
}

// Function to print the help message
void printHelp() {                       //-------------------------------------
    printf("%s\n\n", GREET);
    printf(" Usage: geko [options] \n");   
    printf(" Options: \n");    
    printf(" [-c] <config file> \n");
    printf(" -s <stimulation protocol file> \n");
    printf(" -r <Number of stimulus repetitions> \n");
    printf(" -i <Inter stimuli interval> \n");
    printf(" [-x] <Suffix for output file> \n");
} // end of printHelp() --------------------------------------------------------


float64 stimArray[15000];
// Function to generate the stimulation array, based on the filename supplied
void generateStimArray() {      //-----------------------------------------------
    // stimFile
    // stimArray = stimgen(stim);

    // Assign to the variable stimArray the value of zero vector
    // of length 1 s * sampling rate
    printf("Generating stimulation array...\n");
    int i;
    for (i = 0; i < config.srate; i++)
    {
        stimArray[i] = 0;
    }    
    int32 size_stim = sizeof(stimArray)/sizeof(stimArray[0]);
    // Multiply stimArray by ao_factor
    for (i = 0; i < size_stim; i++)
    {
        stimArray[i] = stimArray[i]*ao_factor;
    }
    printf("Stimulation array size: %d\n", size_stim);
    return;

} // end of generateStimArray() -------------------------------------------------


// Generalized function to interact with DAQ
void readwriteFinite() {
    // Variables associated with the NIDAQ tasks
    TaskHandle AITaskHandle=0, AOTaskHandle=0;
    int32 read; // How many samples have been read
    int32 written; // How many samples have been written
    int32 sampsPerChan = 15000; //sizeof(stimArray)/sizeof(stimArray[0]); // Number of samples to generate/acquire per channel.
    int32 size_data = sampsPerChan*config.nA*2; // Size of the data array
    //float64 data[size_data]; // Data array to be written
    float64 data[300000]; // Data array to be written
    FILE *fp, *fs, *ft;
    int error=0;
    char errBuff[2048]={'\0'};
    char ch, notes[1000];
   
    FileName = getUniqueFileName();
     
    DAQmxErrChk (DAQmxCreateTask("",&AITaskHandle));
    DAQmxErrChk (DAQmxCreateAIVoltageChan(AITaskHandle,config.aiCh,"",DAQmx_Val_RSE,minVal,maxVal,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(AITaskHandle,NULL,config.srate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,sampsPerChan));
  
    DAQmxErrChk (DAQmxCreateTask("",&AOTaskHandle));   
    DAQmxErrChk (DAQmxCreateAOVoltageChan(AOTaskHandle,config.aoCh,"",minVal,maxVal,DAQmx_Val_Volts,NULL));
    DAQmxErrChk (DAQmxCfgSampClkTiming(AOTaskHandle,"",config.srate,DAQmx_Val_Rising,DAQmx_Val_FiniteSamps,sampsPerChan));

    // Connect AO start to AI start
    DAQmxErrChk (DAQmxCfgDigEdgeStartTrig(AOTaskHandle, "ai/StartTrigger", DAQmx_Val_Rising));

    // Arm the AO task
    DAQmxErrChk (DAQmxWriteAnalogF64(AOTaskHandle,sampsPerChan,FALSE,timeout,dataLayout, stimArray, &written, NULL));

    // Start the AI task
    printf("Performing the task...\n");
    DAQmxErrChk (DAQmxStartTask(AOTaskHandle));
    DAQmxErrChk (DAQmxStartTask(AITaskHandle));
    
    DAQmxErrChk (DAQmxReadAnalogF64(AITaskHandle,sampsPerChan,timeout,dataLayout,data,size_data,&read,NULL));
    // int32 DAQmxReadAnalogF64 (TaskHandle taskHandle, int32 numSampsPerChan, float64 timeout, bool32 fillMode, float64 readArray[], uInt32 arraySizeInSamps, int32 *sampsPerChanRead, bool32 *reserved);

    DAQmxErrChk (DAQmxClearTask(AOTaskHandle));
    DAQmxErrChk (DAQmxClearTask(AITaskHandle));

    // Multiply data by ai_factor
    int i;
    for (i = 0; i < size_data; i++)
    {
        data[i] = data[i]*ai_factor;
    }

    char *fullFileName = malloc(strlen(FileName)+strlen(FILESUFFIX)+1);
    sprintf(fullFileName,"%s%s",FileName,FILESUFFIX);
     printf("Made it here!\n");
    fp = fopen(fullFileName, "wb");
    fwrite(&data, sizeof(data), 1, fp);
    fclose(fp);

    sprintf(fullFileName,".%s",FileName);
    int result = mkdir(fullFileName, 0777);
    if (result == 0)
    {
        fs = fopen(configFile, "r");
   
        sprintf(fullFileName,".%s/%s",FileName,configFile);
        ft = fopen(fullFileName, "w");
        ch = fgetc(fs);
        while (ch != EOF)
        {
            fputc(ch, ft);
            ch = fgetc(fs);
        }
        fclose(fs);
        fclose(ft);

        fs = fopen(stimFile, "r");
    
        sprintf(fullFileName,".%s/%s",FileName,stimFile);
        ft = fopen(fullFileName, "w");
        ch = fgetc(fs);
        while (ch != EOF)
        {
            fputc(ch, ft);
            ch = fgetc(fs);
        }
        fclose(fs);
        fclose(ft);

    

        printf("Do you have any experimental notes to add? (Press SPACE ENTER to continue)\n");
        // fgets(notes);
        // Get user input
        scanf("%s", notes);
      
        sprintf(fullFileName,".%s/notes.txt",FileName);
        ft = fopen(fullFileName, "w+");
        fputs("[string used]\n", ft);

        fputs(user_prompt, ft);
        fputs("\n\n", ft);
        fputs("[experimental notes]\n", ft);
        fputs(notes, ft);
    }
    else
    {
        printf("Error creating directory\n");
        return;
    }


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



