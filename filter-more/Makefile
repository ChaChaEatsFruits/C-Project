# Makefile for GTK Image Filter Project

# -----------------
#    VARIABLES
# -----------------

# Compiler to use
CC = gcc

# Compiler flags: Get GTK4 includes, enable all warnings, and add debug info
CFLAGS = `pkg-config --cflags gtk4` -Wall -g

# Linker flags: Get GTK4 libraries and add the math library
LIBS = `pkg-config --libs gtk4` -lm

# Find all .c source files in the current directory
SRCS = $(wildcard *.c)

# Generate object file names from source file names (e.g., main.c -> main.o)
OBJS = $(SRCS:.c=.o)

# The name of the final executable program
TARGET = image_editor

# -----------------
#      RULES
# -----------------

# The default rule, executed when you just run "make"
# It depends on the TARGET executable.
all: $(TARGET)

# Rule to link the object files into the final executable
$(TARGET): $(OBJS)
	@echo "==> Linking to create executable..."
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)
	@echo "==> Build complete! Run with ./$(TARGET)"

# Generic rule to compile a .c file into a .o object file
%.o: %.c
	@echo "==> Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# -----------------
#   PHONY TARGETS
# -----------------

# Rule to clean up build files (object files and the executable)
clean:
	@echo "==> Cleaning up build files..."
	rm -f $(OBJS) $(TARGET)

# Declare targets that are not actual files
.PHONY: all clean