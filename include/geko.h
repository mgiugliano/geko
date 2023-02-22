//
//
//
/****************************************/
// Libraries to include
/****************************************/

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

/****************************************/
// Constants
/****************************************/

#define FILESUFFIX ".dat"  // suffix for the output data file
#define VER "0.1.0"
#define GREET "geko v" VER " - geKo electrophysiology Kommander"
#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else
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

/****************************************/
// DAQmx Configuration Options
/****************************************/

float64 minVal = -10.0; // The minimum value, in units, that you expect to generate.
float64 maxVal = 10.0; // The maximum value, in units, that you expect to generate.
float64 timeout = -1; // The amount of time, in seconds, to wait for the function to read the sample(s).
// TODO: NOT RESPONDING
bool32 dataLayout = DAQmx_Val_GroupByScanNumber; // Specifies how the samples are arranged, either interleaved or noninterleaved. Options: DAQmx_Val_GroupByChannel, DAQmx_Val_GroupByScanNumber

/****************************************/
// Global variables
/****************************************/

unsigned int reps = 1;   // number of repetitions of the stimulation protocol
int currrep = 1;         // current repetition
unsigned int isi = 0;   // inter-stimulus interval in seconds
char *suffix = "";    // suffix for the output data file

char *configFile = "geko.ini";  // name of the configuration file
char *stimFile = "default.stim";    // name of the stimulation protocol file
char *FileName = NULL;     // name of the output data file to be created
char *user_prompt = NULL;   // command used to run this program
int ai_factor = 1;
double ao_factor = 1.0;
configuration config;
int silence = 0;
const char *devName;

/****************************************/
// Function prototypes
/****************************************/

char *getUniqueFileName(); // function to generate a unique file name
char *getCommand(int argc, char *argv[]); // function to concatenate the command line arguments into a single string
int handleArgs(int argc, char *argv[]); // function to handle the command line arguments
static int handler(void* user, const char* section, const char* name, const char* value);
int readConfigFile();
void setConversionFactors(); // function to set the conversion factors for the DAQ
void printPars(); // function to print the parameters used by the program
void print_image(FILE *fptr);
void printHelp(); // function to print the help message
double *generateStimArray(char *stimFile); // function to generate the stimulation array
void mkOutputDir(); // function to create the output directory
void doSingleTask(); // function to do a single task
void doTask();
void mkExperimentalLog();

/****************************************/
// Function definitions
/****************************************/

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

char *getCommand(int argc, char *argv[]) {         //---------------------------
// This function concatenates the command line arguments into a single string.
    int string_length = 0;
    int i;
    for (i = 0; i < argc; i++) {
        string_length = string_length + strlen(argv[i]);
    }
    
    char *out = malloc(string_length + 1);
    out[0] = '\0';
    for (i = 0; i < argc; i++) {
        strcat(out, argv[i]);
        strcat(out, " ");
    }
    return out;
} // end of getCommand() -------------------------------------------------------

int handleArgs(int argc, char *argv[]) {    //----------------------------------
// Function to handle by getopts the command line arguments
    int c;     // variable to store the option

    while ((c = getopt(argc, argv, "c:s:hr:i:x:q")) != -1) {
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
            case 'q':
                silence = 1;
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

static int handler(void* user, const char* section, const char* name, const char* value)  { 
// This function is called by the ini parser for each key-value pair in the config file
    configuration* pconfig = (configuration*)user;
    char *eptr;
    char *aiCh;
    char *aoCh;

    #define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0
    if (MATCH("hardware", "device")) {
        devName = strdup(value);
        pconfig->device = devName;
        
    } else if (MATCH("hardware", "aichans")) {
        aiCh = strdup(devName);
        strcat(aiCh,"/");
        strcat(aiCh,strdup(value));
        pconfig->aiCh = aiCh;
    } else if (MATCH("hardware", "aochans")) {
        aoCh = strdup(devName);
        strcat(aoCh,"/");
        strcat(aoCh,strdup(value));
        pconfig->aoCh = aoCh;
    } else if (MATCH("hardware", "srate")) {
        pconfig->srate = atoi(value);
    } else if (MATCH("hardware", "nrchans")) {
        pconfig->nA = atoi(value);
    } else if (MATCH("amplifier", "mode")) {
        pconfig->mode = strdup(value);
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
} // end of handler() ----------------------------------------------------------

int readConfigFile() {                 //---------------------------------------
// This function reads the configuration file and stores the values in the config struct
    if (ini_parse(configFile, handler, &config) < 0) {
        printf("Can't load '%s'\n", configFile);
        return 1;
    }
    return 0;
} // end of readConfigFile() ---------------------------------------------------

void setConversionFactors() {           //---------------------------------------
// This function sets the conversion factors for the analog input and output
    if (strcmp(config.mode, "cc") == 0) {
        ai_factor = config.ai_factor_cc;
        ao_factor = config.ao_factor_cc;
    } else if (strcmp(config.mode, "vc") == 0) {
        ai_factor = config.ai_factor_vc;
        ao_factor = config.ao_factor_vc;
    } else {
        printf("Unknown amplifier mode: %s\n", config.mode);
        exit(1);
    }
} // end of setConversionFactors() --------------------------------------------

void printPars() {                       //-------------------------------------
// Function to print the parameters used by the program, separated by ========
    FILE *fptr = NULL;
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

void print_image(FILE *fptr) {         //-------------------------------------
    char read_string[128];
    while(fgets(read_string,sizeof(read_string),fptr) != NULL)
        printf("%s",read_string);
} // end of print_image() -----------------------------------------------------

void printHelp() {                       //-------------------------------------
// Function to print the help message
    printf("%s\n\n", GREET);
    printf(" Usage: geko [options] \n");   
    printf(" Options: \n");    
    printf(" [-c] <config file> \n");
    printf(" -s <stimulation protocol file> \n");
    printf(" -r <Number of stimulus repetitions> \n");
    printf(" -i <Inter stimuli interval> \n");
    printf(" [-x] <Suffix for output file> \n");
    printf(" [-q] Silence mode \n");
} // end of printHelp() --------------------------------------------------------

double *generateStimArray(char *stimFile) {      //-----------------------------------------------
    // stimFile
    // stimArray = stimgen(stim);
    static double outArray[15000];
    printf("Generating stimulation array...\n");
    int i;
    for (i = 0; i < config.srate; i++)
    {
        outArray[i] = 0;
    }    

    int size_stim = sizeof(outArray)/sizeof(outArray[0]);
    // Multiply stimArray by ao_factor
    for (i = 0; i < size_stim; i++)
    {
        outArray[i] = outArray[i]*ao_factor;
    }
    return outArray;

} // end of generateStimArray() -------------------------------------------------

void mkOutputDir() {                     //---------------------------------------
// This function creates the output directory
    FileName = getUniqueFileName();
    char *extraFileName = malloc(2*strlen(FileName)+3);
    sprintf(extraFileName,"%s/.%s", FileName,FileName);

    int result = mkdir(FileName, 0777);
    if (result != 0)
    {
        printf("Error creating directory\n");
        return;
    }

    result = mkdir(extraFileName, 0777);
    if (result != 0)
    {
        printf("Error creating directory\n");
        return;
    }
} // end of mkOutputDir() -----------------------------------------------------

void doSingleTask() {                    //---------------------------------------
// This function performs a single task, i.e. a single stimulation protocol
    float64 data[300000]; // Data array to be written
    char *fullFileName = malloc(200);
    FILE *fp;
    int error=0;
    char errBuff[2048]={'\0'};
    // Variables associated with the NIDAQ tasks
    TaskHandle AITaskHandle=0, AOTaskHandle=0;
    int32 read; // How many samples have been read
    int32 written; // How many samples have been written
    int32 sampsPerChan = 15000; //sizeof(stimArray)/sizeof(stimArray[0]); // Number of samples to generate/acquire per channel.
    int32 size_data = sampsPerChan*config.nA*2; // Size of the data array
    
    // Generate stimulation array
    double *stimArray = generateStimArray(stimFile);
    printf("Performing repetition %d of %d...\n", currrep, reps);
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
    DAQmxErrChk (DAQmxStartTask(AOTaskHandle));
    DAQmxErrChk (DAQmxStartTask(AITaskHandle));
    DAQmxErrChk (DAQmxReadAnalogF64(AITaskHandle,sampsPerChan,timeout,dataLayout,data,size_data,&read,NULL));
    DAQmxErrChk (DAQmxClearTask(AOTaskHandle));
    DAQmxErrChk (DAQmxClearTask(AITaskHandle));

    // Multiply data by ai_factor
    int i;
    for (i = 0; i < size_data; i++)
    {
        data[i] = data[i]*ai_factor;
    }
    
    sprintf(fullFileName,"%s/%s_%i%s",FileName,FileName,currrep,FILESUFFIX);
    fp = fopen(fullFileName, "wb");
    fwrite(data, sizeof(data), 1, fp);
    fclose(fp);
   
    printf("Data saved to %s\n\n", fullFileName);

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
      
} // end of doSingleTask() ----------------------------------------------------

void mkExperimentalLog() {               //---------------------------------------
    FILE  *fs1, *ft1, *fs2, *ft2, *ft = NULL;
    char ch1, ch2, notes[1000];
    char *fullFileName = malloc(300);

    fs1 = fopen(configFile, "r");
    sprintf(fullFileName,"%s/.%s/%s",FileName,FileName,configFile);
    
    ft1 = fopen(fullFileName, "w");
    ch1 = fgetc(fs1);
    
    while (ch1 != EOF)
    {
        fputc(ch1, ft1);
        ch1 = fgetc(fs1);
    }
    fclose(fs1);
    fclose(ft1);

    fs2 = fopen(stimFile, "r");
    sprintf(fullFileName,"%s/.%s/%s",FileName,FileName,stimFile);
    ft2 = fopen(fullFileName, "w");
    ch2 = fgetc(fs2);
    while (ch2 != EOF) 
    {
        fputc(ch2, ft2);
        ch2 = fgetc(fs2);
    }
    fclose(fs2);
    fclose(ft2);

    printf("Do you have any experimental notes to add? (Press ENTER to continue)\n");
    // Get user input
    fgets(notes, 1000, stdin);

    sprintf(fullFileName,"%s/.%s/notes.txt",FileName,FileName);
    ft = fopen(fullFileName, "w+");
    fputs("[string used]\n", ft);

    fputs(user_prompt, ft);
    fputs("\n\n", ft);
    fputs("[experimental notes]\n", ft);
    fputs(notes, ft);


    printf("Task completed.\n");

} // end of mkExperimentalLog() -----------------------------------------------

void doTask() {                          //---------------------------------------
// This function performs the task, i.e. the stimulation protocol
    setConversionFactors();
    mkOutputDir();
    int i;
    for (i = 0; i < reps; i++)
    {
        currrep = i + 1;
        doSingleTask();
        sleep(isi);
    }
    mkExperimentalLog();
} // end of doTask() ----------------------------------------------------------