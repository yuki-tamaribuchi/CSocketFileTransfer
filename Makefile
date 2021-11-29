all: makedir server client 
makedir:
	rm -rf build
	mkdir build
server: server.c
	gcc -o build/server.o server.c
client: client.c
	gcc -o build/client.o client.c
