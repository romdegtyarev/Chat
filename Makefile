all: server 

server : server.o
	gcc -std=c99 -o server server.o -lpthread

server.o : server.c
	gcc -std=c99 -c server.c

run:
	xterm -e ./server localhost 57000 &

clean :
	rm -f *.o server