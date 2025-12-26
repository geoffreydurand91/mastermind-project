#include "common.h"
#include <time.h>
#include <signal.h>
#include <sys/wait.h>

// fonction pour generer le code secret
void generer_code(int *code) {
    srand(time(NULL) ^ getpid()); // on utilise le pid pour varier la graine aleatoire
    for(int i = 0; i < 4; i++) {
        code[i] = rand() % 10;
    }
    printf("secret genere (process %d): %d %d %d %d\n", getpid(), code[0], code[1], code[2], code[3]);
}

// fonction logique identique
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

// fonction dediee a la gestion d'un client
void gerer_client(int newsockfd) {
    MastermindReq req;
    MastermindRep rep;
    int secret[4];
    
    generer_code(secret);

    while(1) {
        int n = read(newsockfd, &req, sizeof(req));
        if (n <= 0) {
            printf("fin de connexion client (process %d)\n", getpid());
            break;
        }

        evaluer_proposition(secret, req.combinaison, &rep);
        write(newsockfd, &rep, sizeof(rep));

        if(rep.code_etat == 1) break;
    }
    close(newsockfd);
    exit(0); // fin du processus fils
}

int main() {
    int sockfd, newsockfd;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;
    int opt = 1;

    // evite les processus zombies
    signal(SIGCHLD, SIG_IGN);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) exit(1);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("erreur bind");
        exit(1);
    }

    listen(sockfd, 5);
    printf("serveur multi-clients en attente sur le port %d...\n", PORT);

    while(1) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        
        if (newsockfd < 0) {
            perror("erreur accept");
            continue;
        }

        printf("nouveau client connecte\n");

        int pid = fork();
        if (pid < 0) {
            perror("erreur fork");
        }
        else if (pid == 0) {
            // processus fils
            close(sockfd); // le fils n'a pas besoin d'ecouter
            gerer_client(newsockfd);
        }
        else {
            // processus pere
            close(newsockfd); // le pere n'a pas besoin de la connexion active
        }
    }
    return 0; 
}