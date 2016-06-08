CC = g++
EXEC = webserver
CPP = ./src/*.cpp

all:
	@-$(CC) $(CPP) -o $(EXEC) -I ./include/  -std=c++11 -pthread

clean:
	rm webserver

