// http://hades.mech.northwestern.edu/index.php/NI-DAQ_Cards_on_Linux

#include <stdio.h>
#include <NIDAQmx.h>
#include <time.h>

#define DAQmxErrChk(functionCall) if( DAQmxFailed(error=(functionCall)) ) goto Error; else

int main(void){
	int32		error=0;
	TaskHandle	taskHandle=0;
	int32		written;
	float64		data;
	char		errBuff[2048]={'\0'};
	int		ii;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk(DAQmxCreateTask("Task",&taskHandle));
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle,"Dev1/ai0","ChannelName",DAQmx_Val_RSE,-10.0,10.0,DAQmx_Val_Volts,NULL));
	// To write more analog outputs, simply add more channels to the task.

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk(DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	for(ii = 0; ii < 2000; ii++){
		data = ii % 10;
		DAQmxErrChk(DAQmxWriteAnalogF64(taskHandle,1,TRUE,10.0,DAQmx_Val_GroupByChannel,&data,&written,NULL));
		printf("%4d\t%f\n", ii, data);
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





