all: server

server: server.o HTTPServer.o sha1.o base64.o
	gcc server.o HTTPServer.o sha1.o base64.o -o server

server.o: server.c server.h
	gcc -c server.c

HTTPServer.o: HTTPServer.c HTTPServer.h
	gcc -c HTTPServer.c

sha1.o: sha1.c sha1.h
	gcc -c sha1.c

base64.o: base64.c base64.h
	gcc -c base64.c

clean:
	rm -rf *o server
