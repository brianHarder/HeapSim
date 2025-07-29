CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -Isrc
TARGET = heapsim

SOURCES = src/cli.c src/init.c src/first_fit.c src/best_fit.c src/worst_fit.c src/next_fit.c src/buddy_system.c src/buddy_free.c src/free_block.c src/visualize.c
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

rebuild: clean all

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

run: $(TARGET)
	./$(TARGET)

help:
	@echo "Available targets:"
	@echo "  all       - Build the executable (default)"
	@echo "  clean     - Remove build artifacts"
	@echo "  rebuild   - Clean and rebuild everything"
	@echo "  run       - Build and run the program"
	@echo "  install   - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help      - Show this help message"

.PHONY: all clean rebuild install uninstall run help