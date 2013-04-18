# Makefile of vs-http-server
CC=gcc

IDIR=./include
CFLAGS=-I$(IDIR)

EXECUTABLE = server

CDIR=./src

all: server

server: server.o HTTPServer.o WebSocket.o sha1.o base64.o
	$(CC) server.o HTTPServer.o WebSocket.o sha1.o base64.o -o $(EXECUTABLE)

server.o: $(CDIR)/server.c
	$(CC) -c $(CDIR)/server.c $(CFLAGS)

HTTPServer.o: $(CDIR)/HTTPServer.c
	$(CC) -c $(CDIR)/HTTPServer.c $(CFLAGS)

WebSocket.o: $(CDIR)/WebSocket.c
	$(CC) -c $(CDIR)/WebSocket.c $(CFLAGS)

sha1.o: $(CDIR)/sha1.c
	$(CC) -c $(CDIR)/sha1.c $(CFLAGS)

base64.o: $(CDIR)/base64.c
	$(CC) -c $(CDIR)/base64.c $(CFLAGS)

clean:
	rm -rf *o $(EXECUTABLE)
