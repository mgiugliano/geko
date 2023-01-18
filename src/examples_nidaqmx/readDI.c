// http://hades.mech.northwestern.edu/index.php/NI-DAQ_Cards_on_Linux

#include <stdio.h>
#include <NIDAQmx.h>
#include <time.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void){
	int32		error=0;
	TaskHandle	taskHandle=0;
	int32		read;
	int32		readBytePerSamp;
	int8		data;
	char		errBuff[2048]={'\0'};
	int		ii;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk(DAQmxCreateTask("Task",&taskHandle));
	DAQmxErrChk(DAQmxCreateDIChan(taskHandle,"AnalogIn/port0/line0","",DAQmx_Val_ChanPerLine));
	// Use the line below to add multiple lines from the same port
	// DAQmxErrChk(DAQmxCreateDIChan(taskHandle,"AnalogIn/port0/line0:7","",DAQmx_Val_ChanPerLine));

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk(DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	for(ii = 0; ii < 2000; ii++){
		DAQmxErrChk(DAQmxReadDigitalLines(taskHandle,1,10.0,DAQmx_Val_GroupByScanNumber,&data,1,&read,&readBytePerSamp,NULL));
		printf("%4d\t%d\n", ii, data);
	}

Error:
	if( DAQmxFailed(error) )
		DAQmxGetExtendedErrorInfo(errBuff,2048);
	if( taskHandle!=0 )  {
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if( DAQmxFailed(error) )
		printf("DAQmx Error: %s\n",errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}

