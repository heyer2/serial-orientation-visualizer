#ifndef SERIAL_H
#define SERIAL_H

#include "matrix4.h"

#define SERIAL_BUFFER_SIZE 512
#define SERIAL_LINES 200

struct serialPort {
	int portIndex;
	int baudrate; // NOT REALLY USED??!?!?
	int readIdx;
	int writeIdx;
	int lineIdx;
	char buffer[SERIAL_LINES][SERIAL_BUFFER_SIZE];
	enum oriType {undef, HPR, matrix} representation;
};

void serialStartArgs(struct serialPort* serial, char** argv);
void serialStartManual(struct serialPort* serial);
int serialUpdateBuffer(struct serialPort* serial);
int serialUpdataOrientation(struct serialPort* serial, struct mat4* outputMatrix);
void serialClose(struct serialPort* serial);
void serialEnd(void);

#endif /* SERIAL_H */