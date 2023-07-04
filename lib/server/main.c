#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "server.h"

int main() {
    KeyPair keyPair = generateKeyPair();

    printf("Clé publique :\n%s\n", keyPair.publicKey);
    printf("Clé privée :\n%s\n", keyPair.privateKey);

    printf("Entrez le message chiffré (en hexadécimal) : ");
    char encryptedMessageHex[4096];
    fgets(encryptedMessageHex, sizeof(encryptedMessageHex), stdin);

    int encryptedSize = strlen(encryptedMessageHex) / 2;
    unsigned char* encryptedMessage = (unsigned char*)malloc(encryptedSize);
    memset(encryptedMessage, 0, encryptedSize);

    for (int i = 0; i < encryptedSize; ++i) {
        sscanf(&encryptedMessageHex[i * 2], "%2hhx", &encryptedMessage[i]);
    }

    int decryptedSize = 0;
    unsigned char* decryptedMessage = decryptMessage(encryptedMessage, keyPair.privateKey, encryptedSize, &decryptedSize);

    printf("Message déchiffré :\n%s\n", decryptedMessage);

    free(keyPair.publicKey);
    free(keyPair.privateKey);
    free(encryptedMessage);
    free(decryptedMessage);

    return 0;
}
