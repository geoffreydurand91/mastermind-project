# compilateur et options
CC = gcc
CFLAGS = -Wall -I.

# cibles finales
all: server_tcp client_tcp

# compilation du serveur
server_tcp: server_tcp.o
	$(CC) -o server_tcp server_tcp.o

# compilation du client
client_tcp: client_tcp.o
	$(CC) -o client_tcp client_tcp.o

# regle generique pour les fichiers .o
%.o: %.c common.h
	$(CC) $(CFLAGS) -c $< -o $@

# nettoyage des fichiers generes
clean:
	rm -f *.o server_tcp client_tcp