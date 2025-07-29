# HeapSim Makefile

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc
TARGET = heapsim

# Source files (now in src directory)
SOURCES = src/cli.c src/init.c src/first_fit.c src/best_fit.c src/worst_fit.c src/next_fit.c src/buddy_system.c src/buddy_free.c src/sfree.c src/visualize.c
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Rebuild everything
rebuild: clean all

# Install (copy to /usr/local/bin if you want)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Show help
help:
	@echo "Available targets:"
	@echo "  all       - Build the executable (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and rebuild everything"
	@echo "  run       - Build and run the program"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help message"

# Phony targets
.PHONY: all clean rebuild install uninstall run help