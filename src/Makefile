# the compiler: gcc for C program, define as g++ for C++
  CC = gcc

  # compiler flags:
  #  -g    adds debugging information to the executable file
  #  -Wall turns on most, but not all, compiler warnings
  CFLAGS  = -Wall

  # linking libraries:
  LIBS = -lnidaqmx -lm   

  # the build target executable:
  TARGET = geko

  all: $(TARGET)

  $(TARGET): $(TARGET).c
  	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c $(LIBS)

  clean:
  	$(RM) $(TARGET)

