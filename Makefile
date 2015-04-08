# basic variables
CC = g++
CFLAGS = -c -Wall
LDFLAGS =

# List of sources
SOURCES = src/main.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Executable target
EXECUTABLE = build/ropon

# Opencv specific flags
CFLAGS += `pkg-config --cflags opencv`
LDFLAGS += `pkg-config --libs opencv`

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS) $(EXECUTABLE)
