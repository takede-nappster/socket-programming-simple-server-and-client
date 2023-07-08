#include <stdio.h>
#include <stdlib.h>

#include <netdb.h>
#include <netinet/in.h>

#include <string.h>
#include "lib/client/client.h"

int getkeys(Client *key_dest, int argc, char *argv[])
{
    if (argc != 3)
    {
        return 0;
    }
    if (strcmp(argv[1], "-f") == 0)
    {
        FILE *fp;
        long lSize;
        char *buffer;
        char *filename = argv[2];

        fp = fopen(filename, "rb");
        if (!fp)
            perror(filename), exit(1);

        fseek(fp, 0L, SEEK_END);
        lSize = ftell(fp);
        rewind(fp);

        /* allocate memory for entire content */
        buffer = calloc(1, lSize + 1);
        if (!buffer)
            fclose(fp), fputs("memory alloc fails", stderr), exit(1);

        /* copy the file into the buffer */
        if (1 != fread(buffer, lSize, 1, fp))
            fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);

        /* do your work here, buffer is a string contains the whole text */

        fclose(fp);
        key_dest->public_key = buffer;
    }
    else if (strcmp(argv[1], "-k") == 0)
    {
        key_dest->public_key = argv[2];
    }
    else
    {
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[])
{
    Client client;
    if (getkeys(&client, argc, argv) != 1)
    {
        printf("Paramètres invalide ...\n");
        return 0;
    }

    printf("### Client ###\n");
    printf("# Démarrage du client en cours ...\n");
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;

    struct hostent *server;

    char buffer[256];
    portno = 5001;

    // create socket and get file descriptor
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    server = gethostbyname("127.0.0.1");

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    printf("# Connexion au serveur en cours ... \n");

    // connect to server with server address which is set above (serv_addr)

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR while connecting");
        exit(1);
    }
    printf("# Connexion terminé ...\n");
    printf("# Clé publique : \n%s\n", client.public_key);
    // inside this while loop, implement communicating with read/write or send/recv function
    while (1)
    {
        printf("Entrer votre message : ");
        bzero(buffer, 256);
        scanf("%[^\n]%*c", buffer);

        EncryptedResult result;

        encrypt_message(client, buffer, &result);
        printf("\nLe message chiffré en hexadécimal est le suivant : \n\n");
        printf("==== Message ====\n");
        for (int i = 0; i < result.encrypted_length; i++)
        {
            printf("%02x", result.encrypted_message[i]);
        }
        printf("\n==== Message ====\n\n");

        printf("# Envoie du message chiffré en cours ...\n");

        n = write(sockfd, result.encrypted_message, result.encrypted_length);

        if (n < 0)
        {
            perror("ERROR while writing to socket");
            exit(1);
        }
        
        bzero(buffer, 256);
        n = read(sockfd, buffer, 255);

        if (n < 0)
        {
            perror("ERROR while reading from socket");
            exit(1);
        }

        if(strcmp(buffer, "OK") == 0) {
            printf("# Envoie terminé ... \n\n");
        } else {
            printf("# Erreur de déchiffrage du message ... \n\n");
        }

        // escape this loop, if the server sends message "quit"

        if (!bcmp(buffer, "quit", 4))
            break;
    }
    return 0;
}
