# Compiler and Compiler Flags
CC = gcc
CFLAGS = -Wall -g

# The build target executable:
TARGET = jshell

# Object files
OBJS = command_parser.o JCshell.o process_handler.o utils.o

# The default target
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# To obtain object files
%.o: %.c
	$(CC) $(CFLAGS) -c $<

# To remove generated files
clean:
	rm -f $(TARGET) $(OBJS)
