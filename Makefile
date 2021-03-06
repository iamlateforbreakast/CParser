CC=gcc
RM=rm -f
INCLUDES=src/util
CFLAGS=-g -c -Wall
LDFLAGS= -lsqlite3
SOURCES=src/main.c \
	src/CParser.c \
	src/StringProcessor2.c \
	src/StringBuffer.c \
	src/TransUnit.c \
	src/util/Memory.c \
	src/util/String2.c \
	src/util/FileList.c \
	src/util/FileMgr.c \
	src/util/List.c \
	src/util/Map.c \
	src/util/Debug.c \
        src/util/Object.c \
	src/SdbMgr.c \
	src/Token.c \
	src/OptionMgr.c \
	src/Grammar3.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=cparser

all: $(SOURCES) $(EXECUTABLE)

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -I$(INCLUDES) $< -o $@
