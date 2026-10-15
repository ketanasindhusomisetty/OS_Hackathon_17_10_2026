
CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11 -O2
CPPFLAGS = -D_GNU_SOURCE -Iinclude

TARGET = profiler
SOURCES = src/main.c

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SOURCES) include/profiler.h
	$(CC) $(CPPFLAGS) $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET)
