#include "common.h"
#include <time.h>

// fonction pour generer le code secret
void generer_code(int *code) {
    srand(time(NULL));
    for(int i = 0; i < 4; i++) {
        code[i] = rand() % 10;
    }
    printf("secret genere (udp): %d %d %d %d\n", code[0], code[1], code[2], code[3]);
}

// fonction logique identique au tcp
void evaluer_proposition(int *secret, int *prop, MastermindRep *rep) {
    int secret_temp[4], prop_temp[4];
    rep->bien_places = 0;
    rep->mal_places = 0;

    for(int i = 0; i < 4; i++) {
        secret_temp[i] = secret[i];
        prop_temp[i] = prop[i];
    }

    for(int i = 0; i < 4; i++) {
        if(prop_temp[i] == secret_temp[i]) {
            rep->bien_places++;
            secret_temp[i] = -1;
            prop_temp[i] = -2;
        }
    }

    for(int i = 0; i < 4; i++) {
        if(prop_temp[i] == -2) continue;
        for(int j = 0; j < 4; j++) {
            if(secret_temp[j] == -1) continue;
            if(prop_temp[i] == secret_temp[j]) {
                rep->mal_places++;
                secret_temp[j] = -1;
                break;
            }
        }
    }

    if(rep->bien_places == 4) {
        rep->code_etat = 1;
        strcpy(rep->message, "victoire !");
    } else {
        rep->code_etat = 0;
        strcpy(rep->message, "continuez...");
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t len;
    MastermindReq req;
    MastermindRep rep;
    int secret[4];

    // creation socket udp
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("erreur socket");
        exit(1);
    }

    // configuration adresse
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // bind
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("erreur bind");
        exit(1);
    }

    printf("serveur udp en attente sur le port %d...\n", PORT);
    generer_code(secret);

    while(1) {
        len = sizeof(cli_addr);
        
        // reception du message (bloquant)
        int n = recvfrom(sockfd, &req, sizeof(req), 0, (struct sockaddr *) &cli_addr, &len);
        if (n > 0) {
            printf("reçu de %s: %d %d %d %d\n", inet_ntoa(cli_addr.sin_addr), 
                   req.combinaison[0], req.combinaison[1], req.combinaison[2], req.combinaison[3]);

            // traitement
            evaluer_proposition(secret, req.combinaison, &rep);

            // envoi reponse a l'expediteur
            sendto(sockfd, &rep, sizeof(rep), 0, (struct sockaddr*) &cli_addr, len);
            
            // on reinitialise le jeu si gagne (optionnel, pour permettre de rejouer)
            if(rep.code_etat == 1) {
                printf("partie gagnee par un client. generation nouveau code.\n");
                generer_code(secret);
            }
        }
    }
    return 0;
}