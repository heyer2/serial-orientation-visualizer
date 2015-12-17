#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include "rs232/rs232.h"
#include "headers/serial.h"
#include "headers/matrix4.h"

#define TMP_BUFFER_SIZE 32768

#define ERR_ARG "Error: Unknown program input parameter.\n"
#define ERR_BAUD "Error: Nun-numeric baud rate.\n"
#define ERR_REP "Error: Unknown representation.\n"
#define ERR_PORTNAME "Error: Invalid port name.\n"
#define ERR_PORTAVAIL "Error: Port not available.\n"

#define INFO_PORTREADY "Port successfully opened \n"

#define ELEMENTS_HPR 3
#define ELEMENTS_MATRIX 9

#define ELEMENTSIZE_FLOAT 4
#define ELEMENTSIZE_INT16 2
#define ELEMENTSIZE_INT32 4

int stringIsNumeric(char * str) 
{
    while(*str != '\0')
    {
        if(*str < '0' || *str > '9')
            return 0;
        str++;
    }
    return 1;
}

int string2Int(char * str)
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

int stringIsEqual(char * str1, char * str2)
{
	int i = -1;
	do {
		i++;
		if (str1[i] != str2[i])
			return 0;
	} while (str1[i] != '\0');
	
	return 1;
}

void serialSetBaud(struct serialPort * serial, char * arg)
{
	if (stringIsNumeric(arg))
		serial->baud = string2Int(arg);
	else {
		fputs("ERR_BAUD", stderr);
		exit(EXIT_FAILURE);
	}
}

void serialSetOrientation(struct serialPort * serial, char * arg)
{
	if (stringIsEqual(arg, "HPR"))
		serial->oriRep = HPR;
	else if (stringIsEqual(arg, "matrix"))
		serial->oriRep = matrix;
	else {
		fputs(ERR_REP, stderr);
		exit(EXIT_FAILURE);
	}
}

void serialSetNumber(struct serialPort * serial, char * arg)
{
	if (stringIsEqual(arg, "floating"))
		serial->numRep = floating;
	else if (stringIsEqual(arg, "b16q15"))
		serial->numRep = b16q15;
	else if (stringIsEqual(arg, "b32q29"))
		serial->numRep = b32q29;
	else if (stringIsEqual(arg, "b32q31"))
		serial->numRep = b32q31;
	else {
		fputs(ERR_REP, stderr);
		exit(EXIT_FAILURE);
	}
}

void serialOpen(struct serialPort * serial, char * arg)
{	
	comEnumerate();
	int index = comFindPort(arg);
	int status;
	
	if (index != -1)
		status = comOpen(index, serial->baud);
	else {
		fputs(ERR_PORTNAME, stderr);
		exit(EXIT_FAILURE);
	}
	
	if (status == 0) {
		fputs(ERR_PORTAVAIL, stderr);
		exit(EXIT_FAILURE);
	}

	serial->portIndex = index;
	serial->bufferIdx = 0;
	serial->flagNewOrientation = 0;
	serial->flagDesync = 1;
	mat4Eye(&serial->matOri);
	fputs(INFO_PORTREADY, stdout);
}

void serialInitiate(struct serialPort * serial, int argc, char ** argv)
{	
	for (int i = 0; i < argc - 1; i++) {
		if (argv[i][0] == '-')
			switch (argv[i][1]) {
				case 'b': serialSetBaud(serial, argv[i + 1]);		 break;
				case 'o': serialSetOrientation(serial, argv[i + 1]); break;
				case 'n': serialSetNumber(serial, argv[i + 1]);		 break;
				default : fputs(ERR_ARG, stderr); exit(EXIT_FAILURE);break;
			}
	}
	serialOpen(serial, argv[argc-1]);
}

double serialRawToDouble(struct serialPort * serial, void * ptr)
{
	switch(serial->numRep) {
		case floating: return *(float*)ptr;
		case b16q15  : return *(int16_t*)ptr / pow(2.0, 15);
		case b32q29  : return *(int32_t*)ptr / pow(2.0, 29);
		case b32q31  : return *(int32_t*)ptr / pow(2.0, 31);
	}
}

void serialMatrixFromPackage(struct serialPort * serial, char * packet)
{
	int elementSize;
	switch(serial->numRep) {
		case floating: elementSize = ELEMENTSIZE_FLOAT;   break;
		case b16q15  : elementSize = ELEMENTSIZE_INT16; break;
		case b32q29  : elementSize = ELEMENTSIZE_INT32; break;
		case b32q31  : elementSize = ELEMENTSIZE_INT32; break;
	}

	int elements;
	switch(serial->oriRep) {
		case HPR   : elements = ELEMENTS_HPR;  	 break;
		case matrix: elements = ELEMENTS_MATRIX; break;
	}

	double packetData[elements];

	for (int i = 0; i < elements; i++) {
		void * ptr = packet + i * elementSize;
		packetData[i] = serialRawToDouble(serial, ptr);
	}

	switch(serial->oriRep) {
		case HPR:
			mat4SetRotFromHPR(&serial->matOri, packetData[0], packetData[1], packetData[2]);
			break;
		case matrix:
			for (int i = 0; i < ELEMENTS_MATRIX; i++)
				serial->matOri.data[i/3][i%3] = packetData[i];
			break;
	}
}

int serialResync(struct serialPort * serial)
{	
	char byte;
	comRead(serial->portIndex, &byte, 1);
	if (byte == '\n') {
		serial->flagDesync = 0;
		serial->bufferIdx = 0;
		return 1;
	}
	else
		return 0;
}

int serialUpdate(struct serialPort * serial)
{	
	if (serial->flagDesync && !serialResync(serial))
		return 0;

	int packetCount = 0;
	
	int packetSize = 1;
	switch(serial->oriRep) {
		case HPR   : packetSize *= ELEMENTS_HPR;    break;
		case matrix: packetSize *= ELEMENTS_MATRIX; break;
	}
	switch(serial->numRep) {
		case floating: packetSize *= ELEMENTSIZE_FLOAT; break;
		case b16q15  : packetSize *= ELEMENTSIZE_INT16; break;
		case b32q29  : packetSize *= ELEMENTSIZE_INT32; break;
		case b32q31  : packetSize *= ELEMENTSIZE_INT32; break;
	}

	char packet[packetSize + 1]; // End of packet marker, newline
	char tmpBuffer[TMP_BUFFER_SIZE];
	int readChars = comRead(serial->portIndex, tmpBuffer, TMP_BUFFER_SIZE);
	for (int i = 0; i < readChars; i++) {
		serial->buffer[serial->bufferIdx] = tmpBuffer[i];
		if (serial->bufferIdx == packetSize) {
			for (int j = 0; j <= packetSize; j++)
				packet[j] = serial->buffer[j];
			serial->bufferIdx = 0;
			packetCount++;
		}
		else
			serial->bufferIdx++;
	}
	if (packet[packetSize] != '\n') {
		serial->flagDesync = 1;
		return 0;
	}
	serialMatrixFromPackage(serial, packet);
	serial->flagNewOrientation = 1;
	return packetCount;
}

void serialClose(struct serialPort * serial)
{
	comClose(serial->portIndex);
}

void serialEnd(void)
{
	comTerminate();
}
