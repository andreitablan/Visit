#fisier folosit pentru compilarea serverului&clientului TCP concurent

all:
	gcc server.c -o server
	gcc client.c -o client
clean:
	rm -f client server
