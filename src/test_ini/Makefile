# Makefile to compile this example

# Compiler
CC = gcc

# Compiler flags
#CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -g
CFLAGS = -Wall -pedantic -std=c99 -g

# Linker flags
LDFLAGS = -lm

# Name of the executable
EXEC = main

# List of object files
OBJS = main.o ini.o

# List of header files
HDRS = main.h ini.h

# Default target
all: $(EXEC)

# Linking
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC) $(LDFLAGS)

# Compiling
main.o: main.c $(HDRS)
	$(CC) $(CFLAGS) -c main.c

ini.o: ini.c $(HDRS)
	$(CC) $(CFLAGS) -c ini.c

# Cleaning
clean:
	rm -f $(OBJS) $(EXEC)

# End of Makefile
