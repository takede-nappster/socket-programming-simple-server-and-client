#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include "lib/server/server.h"

void bzero(void *a, size_t n) {
    memset(a, 0, n);
}

void bcopy(const void *src, void *dest, size_t n) {
    memmove(dest, src, n);
}

struct sockaddr_in* init_sockaddr_in(uint16_t port_number) {
    struct sockaddr_in *socket_address = malloc(sizeof(struct sockaddr_in));
    memset(socket_address, 0, sizeof(*socket_address));
    socket_address -> sin_family = AF_INET;
    socket_address -> sin_addr.s_addr = htonl(INADDR_ANY);
    socket_address -> sin_port = htons(port_number);
    return socket_address;
}

char* process_operation(char *input) {
    size_t n = strlen(input) * sizeof(char);
    char *output = malloc(n);
    memcpy(output, input, n);
    return output;
}

int main( int argc, char *argv[] ) {

    const uint16_t port_number = 5001;
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    printf("### Serveur  ### \n");
    printf("# Démarrage en cours ...\n");

    struct sockaddr_in *server_sockaddr = init_sockaddr_in(port_number);
    struct sockaddr_in *client_sockaddr = malloc(sizeof(struct sockaddr_in));
    socklen_t server_socklen = sizeof(*server_sockaddr);
    socklen_t client_socklen = sizeof(*client_sockaddr);


    if (bind(server_fd, (const struct sockaddr *) server_sockaddr, server_socklen) < 0)
    {
        printf("Error! Bind has failed\n");
        exit(0);
    }
    if (listen(server_fd, 3) < 0)
    {
        printf("Error! Can't listen\n");
        exit(0);
    }
    const size_t buffer_len = 256;
    unsigned char *buffer = malloc(buffer_len * sizeof(char));
    char *response = NULL;
    time_t last_operation;
    __pid_t pid = -1;
    printf("# Démarrage terminé ...\n");
    printf("# Génération de la paire de clé en cours ... \n");
    KeyPair keyPair = generateKeyPair();
    printf("Clé publique :\n%s\n", keyPair.publicKey);
    printf("Clé privée :\n%s\n", keyPair.privateKey);


    while (1) {
        if (pid == -1) 
            printf("# En attente d'une nouvelle connexion (Ctrl+C pour quitter) ...\n\n");
        else
            printf("# En attente de message (Ctrl+C pour quitter) ... \n\n");
        int client_fd = accept(server_fd, (struct sockaddr *) &client_sockaddr, &client_socklen);

        pid = fork();

        if (pid == 0) {
            close(server_fd);

            if (client_fd == -1) {
                exit(0);
            }

            printf("# Connexion établie ...\n");
            printf("# En attente de message (Ctrl+C pour quitter) ... \n");

            last_operation = clock();

            while (1) {
                read(client_fd, buffer, buffer_len);

                if (buffer == "__close__") {
                    close(client_fd);
                    printf("# Fermeture de la session. Bye!\n", client_fd);
                    break;
                }

                if (strlen(buffer) == 0) {
                    clock_t d = clock() - last_operation;
                    double dif = 1.0 * d / CLOCKS_PER_SEC;

                    if (dif > 5.0) {
                        printf("Process %d: ", getpid());
                        close(client_fd);
                        printf("Connection timed out after %.3lf seconds. ", dif);
                        printf("Closing session with `%d`. Bye!\n", client_fd);
                        break;
                    }

                    continue;
                }
                printf("# Message chiffré reçu : \n");
                printf("==== Message chiffré ==== \n");
                for (int i = 0; i < 256; i++)
                {
                    printf("%02x", buffer[i]);
                }

                printf("\n==== Message chiffré ==== \n");
                printf("# Déchiffrement en cours ... \n");
                int decryptedSize;
                unsigned char* decryptedMessage = decryptMessage(buffer, keyPair.privateKey, 256, &decryptedSize);
                printf("# Déchiffrement terminé ...");
                printf("# Le message en claire est le suivant : \n");
                printf("==== Message clair ==== \n");
                printf("\n%s\n", decryptedMessage);
                printf("\n==== Message clair ==== \n\n");
                printf("# Fin du traitement ..\n\n");
                printf("# En attente de nouveau messages ...\n\n");


                free(response);
                //response = process_operation(buffer);
                response = "OK";
                bzero(buffer, buffer_len * sizeof(char));

                send(client_fd, response, strlen(response), 0);
                //printf("Responded with `%s`. Waiting for a new query...\n", response);

                last_operation = clock();
            }
            exit(0);
        }
        else {
            close(client_fd);
        }
    }
}