#include "common.h"

int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    socklen_t len;
    MastermindReq req;
    MastermindRep rep;
    char *server_ip = "127.0.0.1";

    if (argc > 1) {
        server_ip = argv[1];
    }

    // creation socket udp
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("erreur socket");
        exit(1);
    }

    // configuration de l'adresse du serveur destinataire
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        perror("adresse invalide");
        exit(1);
    }

    printf("client udp pret (cible: %s:%d)\n", server_ip, PORT);
    printf("entrez 4 chiffres separes par des espaces (ex: 1 2 3 4)\n");

    while(1) {
        printf("votre proposition > ");
        int r = scanf("%d %d %d %d", &req.combinaison[0], &req.combinaison[1], &req.combinaison[2], &req.combinaison[3]);

        if (r != 4) {
            printf("format invalide.\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        len = sizeof(serv_addr);

        // envoi direct
        sendto(sockfd, &req, sizeof(req), 0, (struct sockaddr *) &serv_addr, len);

        // attente reponse
        int n = recvfrom(sockfd, &rep, sizeof(rep), 0, NULL, NULL);
        if (n < 0) {
            perror("erreur reception");
            break;
        }

        printf("resultat: %d bien places, %d mal places. message: %s\n", 
               rep.bien_places, rep.mal_places, rep.message);

        if (rep.code_etat == 1) {
            printf("gagne !\n");
            break;
        }
    }

    close(sockfd);
    return 0;
}