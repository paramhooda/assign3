CC = g++
CFLAGS = -c -Wall -std=c++11 -O2
all = server client

server: server.o tands.o
	$(CC) -O -Wall server.o tands.o -o server

client: client.o tands.o
	$(CC) -O -Wall client.o tands.o -o client

server.o: server.cpp server.h
	$(CC) server.cpp -o server.o $(CFLAGS)

client.o: client.cpp client.h
	$(CC) client.cpp -o client.o $(CFLAGS)

tands.o: tands.c tands.h
	g++ tands.c -o tands.o $(CFLAGS)

clean:
	@rm -rf *.o
