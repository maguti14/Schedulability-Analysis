# the compiler: gcc for C program, define as g++ for C++
#CC = gcc


CFLAGS  = -std=gnu11 -Wall
# the build target executable:
TARGET = homework2

all:
	gcc $(CFLAGS) -o $(TARGET) $(TARGET).c $(TARGET).h -lm

clean:
	$(RM) $(TARGET)
