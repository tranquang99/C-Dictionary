# change application name here (executable output name)
TARGET=dictionary

defaut: run
# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall
 
PTHREAD=-pthread
 
CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe
 
GTKLIB=`pkg-config --cflags --libs gtk+-3.0`
 
# linker
LD=gcc
LDFLAGS= $(PTHREAD) $(GTKLIB) Btree/lib/libbt.a -export-dynamic
 
OBJS=main.o
 
all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
    
main.o: main.c
	$(CC) -c $(CCFLAGS) main.c $(GTKLIB) -o main.o

run: all $(OBJS)
	./dictionary

clean:
	rm -f *.o $(TARGET)
