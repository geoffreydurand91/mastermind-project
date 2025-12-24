#include "common.h"
#include <time.h>

// fonction pour generer le code secret
void generer_code(int *code) {
    srand(time(NULL));
    for(int i = 0; i < 4; i++) {
        code[i] = rand() % 10; // chiffres entre 0 et 9
    }
    printf("secret genere (serveur): %d %d %d %d\n", code[0], code[1], code[2], code[3]);
}

// fonction pour evaluer la proposition
void evaluer_proposition(int *secret, int *prop, MastermindRep *rep) {
    int secret_temp[4], prop_temp[4];
    rep->bien_places = 0;
    rep->mal_places = 0;

    // copie pour modification
    for(int i = 0; i < 4; i++) {
        secret_temp[i] = secret[i];
        prop_temp[i] = prop[i];
    }

    // calcul des bien places
    for(int i = 0; i < 4; i++) {
        if(prop_temp[i] == secret_temp[i]) {
            rep->bien_places++;
            secret_temp[i] = -1; // marque comme traite
            prop_temp[i] = -2;   // marque comme traite
        }
    }

    // calcul des mal places
    for(int i = 0; i < 4; i++) {
        if(prop_temp[i] == -2) continue; // deja bien place

        for(int j = 0; j < 4; j++) {
            if(secret_temp[j] == -1) continue; // deja utilise

            if(prop_temp[i] == secret_temp[j]) {
                rep->mal_places++;
                secret_temp[j] = -1; // marque comme utilise
                break;
            }
        }
    }

    // mise a jour etat
    if(rep->bien_places == 4) {
        rep->code_etat = 1; // gagne
        strcpy(rep->message, "victoire !");
    } else {
        rep->code_etat = 0; // continue
        strcpy(rep->message, "continuez...");
    }
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    MastermindReq req;
    MastermindRep rep;
    int secret[4];
    int opt = 1;

    // creation socket tcp
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("erreur ouverture socket");
        exit(1);
    }

    // option pour relancer le serveur rapidement sans erreur
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // configuration adresse
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // attachement 
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("erreur bind");
        exit(1);
    }

    // ecoute
    listen(sockfd, 5);
    printf("serveur tcp en attente sur le port %d...\n", PORT);

    clilen = sizeof(cli_addr);
    
    // acceptation d'un client (bloquant)
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        perror("erreur accept");
        exit(1);
    }
    printf("client connecte\n");

    // generation du jeu
    generer_code(secret);

    // boucle de jeu
    while(1) {
        // lecture de la proposition
        int n = read(newsockfd, &req, sizeof(req));
        if (n <= 0) {
            printf("client deconnecte\n");
            break;
        }

        printf("reçu: %d %d %d %d\n", req.combinaison[0], req.combinaison[1], req.combinaison[2], req.combinaison[3]);

        // traitement
        evaluer_proposition(secret, req.combinaison, &rep);

        // envoi reponse
        write(newsockfd, &rep, sizeof(rep));

        if(rep.code_etat == 1) {
            printf("partie gagnee, fin de connexion.\n");
            break;
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}