CC=gcc
RM=rm -f
INCLUDES=src/util
CFLAGS=-g -c -Wall
LDFLAGS=
SOURCES=src/main.c \
	src/CParser.c \
	src/StringProcessor.c \
	src/StringBuffer.c \
	src/util/Memory.c \
	src/util/String.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=cparser

all: $(SOURCES) $(EXECUTABLE)

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -I$(INCLUDES) $< -o $@
