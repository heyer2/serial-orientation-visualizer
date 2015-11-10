#ifndef SERIAL_H
#define SERIAL_H

#include "matrix4.h"

#define SERIAL_BUFFER_SIZE 512

struct serialPort {
	int flagDesync;
	int flagNewOrientation;
	struct mat4 matOri;
	int portIndex;
	int baud;
	int bufferIdx;
	char buffer[SERIAL_BUFFER_SIZE];
	enum oriType {HPR, matrix} oriRep;
	enum numberType {floating, fixed16, fixed32} numRep;
};


void serialSetBaud(struct serialPort * serial, char * arg);
void serialSetOrientation(struct serialPort * serial, char * arg);
void serialSetNumber(struct serialPort * serial, char * arg);
void serialOpen(struct serialPort * serial, char * arg);
void serialInitiate(struct serialPort * serial, int argc, char ** argv);
int serialResync(struct serialPort * serial);
double serialRawToDouble(struct serialPort * serial, void * ptr);
void serialMatrixFromPackage(struct serialPort * serial, char * package);
int serialUpdate(struct serialPort * serial);
void serialClose(struct serialPort * serial);
void serialEnd(void);

#endif /* SERIAL_H */