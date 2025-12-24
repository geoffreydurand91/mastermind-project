#include "common.h"

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    MastermindReq req;
    MastermindRep rep;

    // verification des arguments 
    char *server_ip = "127.0.0.1";
    if (argc > 1) {
        server_ip = argv[1];
    }

    // creation socket tcp
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("erreur ouverture socket");
        exit(1);
    }

    // configuration de l'adresse du serveur
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("adresse invalide");
        exit(1);
    }

    // connexion
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("erreur connexion");
        exit(1);
    }
    printf("connecte au serveur %s sur le port %d\n", server_ip, PORT);
    printf("entrez 4 chiffres separes par des espaces (ex: 1 2 3 4)\n");

    // boucle de jeu
    while(1) {
        printf("votre proposition > ");
        int r = scanf("%d %d %d %d", &req.combinaison[0], &req.combinaison[1], &req.combinaison[2], &req.combinaison[3]);
        
        if (r != 4) {
            printf("format invalide. reessayez.\n");
            // vider le buffer clavier
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        // envoi de la proposition
        if (write(sockfd, &req, sizeof(req)) < 0) {
            perror("erreur ecriture");
            break;
        }

        // lecture de la reponse
        if (read(sockfd, &rep, sizeof(rep)) < 0) {
            perror("erreur lecture");
            break;
        }

        // affichage resultat
        printf("resultat: %d bien places, %d mal places. message: %s\n", 
               rep.bien_places, rep.mal_places, rep.message);

        // verification fin de partie
        if (rep.code_etat == 1) {
            printf("bravo, vous avez trouve !\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}