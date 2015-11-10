CC = gcc
CFLAGS = -I. -Wall
OBJECTS = main.o matrix4.o serial.o graphics.o interface.o rs232-unix.o
TARGET = visualizer.out
LIBS= -lglew -lglfw3 -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LIBS)

main.o : main.c
	$(CC) $(CFLAGS) -c main.c

matrix4.o : matrix4.c
	$(CC) $(CFLAGS) -c matrix4.c

serial.o : serial.c
	$(CC) $(CFLAGS) -c serial.c

graphics.o : graphics.c
	$(CC) $(CFLAGS) -c graphics.c

interface.o : interface.c
	$(CC) $(CFLAGS) -c interface.c

rs232-unix.o : rs232/rs232-unix.c
	$(CC) $(CFLAGS) -c rs232/rs232-unix.c

clean:
	rm -f *.o