all: server

server: server.o HTTPServer.o
	gcc server.o HTTPServer.o -o server

server.o: server.c server.h
	gcc -c server.c

HTTPServer.o: HTTPServer.c HTTPServer.h
	gcc -c HTTPServer.c

clean:
	rm -rf *o server
