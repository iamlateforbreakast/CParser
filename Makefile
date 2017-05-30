CC=gcc
RM=rm -f
INCLUDES=src/util
CFLAGS=-g -c -Wall
LDFLAGS= -lsqlite3
SOURCES=src/main.c \
	src/CParser.c \
	src/StringProcessor.c \
	src/StringBuffer.c \
	src/TransUnit.c \
	src/TokenList.c \
	src/util/Memory.c \
	src/util/String.c \
	src/util/FileList.c \
	src/util/FileMgr.c \
	src/SdbMgr.c \
	src/Token.c \
	src/OptionMgr.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=cparser

all: $(SOURCES) $(EXECUTABLE)

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)
$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

.c.o:
	$(CC) $(CFLAGS) -I$(INCLUDES) $< -o $@
