#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// definition du port et de la taille du buffer
#define PORT 5000
#define BUFFER_SIZE 1024

// structure envoyee par le client : sa proposition
typedef struct {
    int combinaison[4]; // les 4 chiffres proposes
} MastermindReq;

// structure envoyee par le serveur : le resultat
typedef struct {
    int bien_places; // nombre de chiffres bien places
    int mal_places;  // nombre de chiffres presents mais mal places
    int code_etat;   // 0 = en cours, 1 = gagne, -1 = perdu
    char message[BUFFER_SIZE]; // message textuel optionnel
} MastermindRep;

#endif