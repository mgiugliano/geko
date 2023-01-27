//
// Sample program to read analog input from a NI-DAQ card
//
// This is what I would have love to find packaged with NiDAQmx.
//
// Compile with:
// gcc -o readAI readAI.c -lnidaqmx
//

// from http://hades.mech.northwestern.edu/index.php/NI-DAQ_Cards_on_Linux

#include <stdio.h>
#include <NIDAQmx.h>
#include <time.h>

#define DAQmxErrChk(functionCall)            \
	if (DAQmxFailed(error = (functionCall))) \
		goto Error;                          \
	else

int main(void)
{
	int32 error = 0;
	TaskHandle taskHandle = 0;
	int32 read;
	float64 data;
	char errBuff[2048] = {'\0'};
	int ii;

	/*********************************************/
	// DAQmx Configure Code
	/*********************************************/
	DAQmxErrChk(DAQmxCreateTask("Task", &taskHandle));
	DAQmxErrChk(DAQmxCreateAIVoltageChan(taskHandle, "AnalogIn/ai0", "ChannelName", DAQmx_Val_RSE, -10.0, 10.0, DAQmx_Val_Volts, NULL));
	// To read more analog inputs, simply add more channels to the task.

	/*********************************************/
	// DAQmx Start Code
	/*********************************************/
	DAQmxErrChk(DAQmxStartTask(taskHandle));

	/*********************************************/
	// DAQmx Read Code
	/*********************************************/
	for (ii = 0; ii < 2000; ii++)
	{
		DAQmxErrChk(DAQmxReadAnalogF64(taskHandle, 1, 10.0, DAQmx_Val_GroupByChannel, &data, 1, &read, NULL));
		printf("%4d\t%f\n", ii, data);
	}

Error:
	if (DAQmxFailed(error))
		DAQmxGetExtendedErrorInfo(errBuff, 2048);
	if (taskHandle != 0)
	{
		/*********************************************/
		// DAQmx Stop Code
		/*********************************************/
		DAQmxStopTask(taskHandle);
		DAQmxClearTask(taskHandle);
	}
	if (DAQmxFailed(error))
		printf("DAQmx Error: %s\n", errBuff);
	printf("End of program, press Enter key to quit\n");
	getchar();
	return 0;
}
