# compilateur et options
CC = gcc
CFLAGS = -Wall -I.

# cibles finales
all: server_tcp client_tcp server_udp client_udp server_multi

# compilation tcp
server_tcp: server_tcp.o
	$(CC) -o server_tcp server_tcp.o

client_tcp: client_tcp.o
	$(CC) -o client_tcp client_tcp.o

# compilation udp
server_udp: server_udp.o
	$(CC) -o server_udp server_udp.o

client_udp: client_udp.o
	$(CC) -o client_udp client_udp.o

server_multi: server_multi.o
	$(CC) -o server_multi server_multi.o

# regle generique
%.o: %.c common.h
	$(CC) $(CFLAGS) -c $< -o $@

# nettoyage
clean:
	rm -f *.o server_tcp client_tcp server_udp client_udp