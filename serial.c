#include <stdio.h>
#include <math.h>
#include "rs232/rs232.h"
#include "headers/serial.h"
#include "headers/matrix4.h"

#define TERMINAL_BUFFER_SIZE 512

#define TMP_BUFFER_SIZE 2048



/*
void waitForEnter()
{
    printf("Please press \"Enter\" to continue...\n");
    while (fgetc(stdin)!= '\n') {
        fflush(stdin);
        printf("Incorrect input. Kindly press \"Enter\" to continue...\n");
    }
}
*/

char* getInput()
{
	static char input[TERMINAL_BUFFER_SIZE];
	
	for (int i = 0; i < TERMINAL_BUFFER_SIZE; i++) {
		input[i] = fgetc(stdin);
		if (input[i] == 0x0A) {
			input[i] = '\0';
			return input;		
		}
	}
	fputs("Input buffer overflow.", stderr);
	return NULL; // redundant
}

int stringCountSpaces(char * str)
{	
	int i = 0;
	int spaces = 0;
	while (str[i] != '\0') {
		if (str[i] == ' ')
			spaces++;
		i++;
	}
	return spaces;
}

int stringIsNumeric(char* str) 
{
    while(*str != '\0')
    {
        if(*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

int string2Int(char* str)
{
	int i = 0;
	int output = 0;
	
	while (str[i] != '\0')
			i++;
	int digits = i;
	for (i = 0; i < digits; i++)
		output += pow(10, digits-i-1) * (str[i]-'0');

	return output;
}

int stringIsEqual(char* str1, char* str2)
{	
	int i = -1;
	do {
		i++;
		if (str1[i] != str2[i])
			return 0;
	} while (str1[i] != '\0');
	
	return 1;
}

void serialStart(struct serialPort* serial) {
	char* input;
	int status = -1;
	
	comEnumerate();
	
	while (status != 1) {
		if (status == 0) //not first run
			printf("Port unavailable, try again.\n");
		
		printf("Enter name of port to be opened:\n");
		serial->portIndex = comFindPort(getInput());
		while (serial->portIndex == -1) {
			printf("Invalid port name, retry:\n");
			serial->portIndex = comFindPort(getInput());
		}
		printf("Enter the baud rate of the port to be opened:\n");
		input = getInput();
		while (!stringIsNumeric(input)) {
			printf("Not a number, try again:\n");
			input = getInput();
		}
		status = comOpen(serial->portIndex, string2Int(input));
	}
	
	printf("Port successfully opened.\n");
	
	status = -1;
	while (status != 1) {
		if (status == 0) //not first run
			printf("Not among selection, try again.\n");
		
		printf("What is the type of input? Choose among [HPR, matrix]:\n");
		input = getInput();
		if (stringIsEqual(input, "HPR")) {
			serial->representation = HPR;
			status = 1;
		} else if (stringIsEqual(input, "matrix")) {
			serial->representation = matrix;
			status = 1;
		} else
			status = 0;
	}
	serial->lineIdx = 0;
	serial->writeIdx = 0;
	serial->readIdx = 0;
	printf("Port ready for use\n");
}

void serialIncrementReadIdx(struct serialPort* serial)
{
	if (serial->readIdx == SERIAL_LINES - 1)
		serial->readIdx = 0;
	else
		serial->readIdx++;
}

void serialIncrementWriteIdx(struct serialPort* serial)
{
	if (serial->writeIdx == SERIAL_LINES - 1)
		serial->writeIdx = 0;
	else
		serial->writeIdx++;
	if (serial->writeIdx == serial->readIdx)
		serialIncrementReadIdx(serial);
}

void serialIncrementLineIdx(struct serialPort* serial)
{
	if (serial->lineIdx < SERIAL_BUFFER_SIZE - 1)
		serial->lineIdx++;
	else
		fputs("Received too long line!", stderr);
}

int serialUpdateBuffer(struct serialPort* serial)
{
	char tmpString[TMP_BUFFER_SIZE];
	int readChars = comRead(serial->portIndex, tmpString, TMP_BUFFER_SIZE);
	for (int i = 0; i < readChars; i++) {
		if (tmpString[i] == '\n') {
			serial->buffer[serial->writeIdx][serial->lineIdx] = '\0';
			serial->lineIdx = 0;
			serialIncrementWriteIdx(serial);
		} else if (tmpString[i] != '\r') { 	
			serial->buffer[serial->writeIdx][serial->lineIdx] = tmpString[i];
			serialIncrementLineIdx(serial);
		}
	}
	return readChars;
}


int serialUpdataOrientation(struct serialPort* serial, struct mat4* outputMatrix)
{
	if (serial->writeIdx == serial->readIdx)
		return 0;
	switch(serial->representation) {
		case HPR : {
			double heading, pitch, roll;
			int spaces = stringCountSpaces(serial->buffer[serial->readIdx]);
			int conforms = sscanf(serial->buffer[serial->readIdx], "%lf %lf %lf", &heading, &pitch, &roll);
			serialIncrementReadIdx(serial);
			//printf("Conforms: %i Spaces: %i\n", conforms, spaces);
			if (spaces > 3 || conforms != 3)
				return 0;
			mat4SetRotFromHPR(outputMatrix, heading, pitch, roll);
			return 1;
			break;
		}
		case matrix : {
			struct mat4 rotMatrix;
			mat4Init(&rotMatrix);
			int spaces = stringCountSpaces(serial->buffer[serial->readIdx]);
			int conforms = sscanf(serial->buffer[serial->readIdx], "%lf %lf %lf %lf %lf %lf %lf %lf %lf", &(rotMatrix.data[0][0]), &(rotMatrix.data[0][1]), &(rotMatrix.data[0][2]), &(rotMatrix.data[1][0]), &(rotMatrix.data[1][1]), &(rotMatrix.data[1][2]), &(rotMatrix.data[2][0]), &(rotMatrix.data[2][1]), &(rotMatrix.data[2][2]));
			serialIncrementReadIdx(serial);
 			printf("Conforms: %i Spaces: %i\n", conforms, spaces);
			if (spaces > 10 || conforms != 9)
				return 0;
			mat4SetRotFromMat(outputMatrix, &rotMatrix);
			printf("Successfully set rot\n");
			return 1;
			break;
		}
		case undef : 
			fputs("Cannot update orientation using serial object with undefined representation!", stderr);
			break;
		default : 
			fputs("Switch statement fall through error!", stderr);
	}	
	return(0);
}

void serialClose(struct serialPort* serial)
{
	comClose(serial->portIndex);
}

void serialEnd(void)
{
	comTerminate();
}