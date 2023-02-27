# the compiler: gcc for C program, define as g++ for C++
  CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  # CFLAGS  = -Wall

  # linking libraries:
  # LIBS = -lnidaqmx
  LIBS = /usr/lib/x86_64-linux-gnu/libnidaqmx.so.1
  # the build target executable:
  TARGET = readAI   readDI   writeAO  writeDO

  all: $(TARGET)

  $(TARGET): $(TARGET).c
  	$(CC) $(LIBS) -o $(TARGET) $(TARGET).c

  clean:
  	$(RM) $(TARGET)

