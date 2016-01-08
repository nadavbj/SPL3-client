# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
CC = g++
CFLAGS  = -g -Wall
LFLAGS  = -L/usr/lib -lboost_system -lboost_thread

# All Targets
all: spl-client

spl-client: bin/ConnectionHandler.o bin/echoClient.o
	$(CC) -o bin/client bin/ConnectionHandler.o bin/echoClient.o $(LFLAGS)


bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: src/echoClient.cpp
	$(CC) $(CFLAGS) -c -Linclude -o bin/echoClient.o src/echoClient.cpp


clean:
	rm -f bin/*
