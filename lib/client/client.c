#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "client.h" 

void encrypt_message(Client client, const char *message, EncryptedResult * result) {
    BIO *bio_public = BIO_new_mem_buf(client.public_key, -1);
    RSA *rsa_public = PEM_read_bio_RSAPublicKey(bio_public, NULL, NULL, NULL);
    BIO_free(bio_public);

    if (rsa_public == NULL) {
        printf("Erreur lors de la lecture de la clé publique.\n");
        return;
    }

    int message_length = strlen(message);
    int rsa_size = RSA_size(rsa_public);
    int encrypted_length = 0;
    unsigned char *encrypted_message = malloc(rsa_size);

    encrypted_length = RSA_public_encrypt(message_length, (unsigned char *)message, encrypted_message, rsa_public, RSA_PKCS1_PADDING);

    printf("Message chiffré (en hexadécimal):\n");
    for (int i = 0; i < encrypted_length; i++) {
        printf("%02x", encrypted_message[i]);
    }
    printf("\n");


    RSA_free(rsa_public);
    //free(encrypted_message);
    result->encrypted_length = encrypted_length;
    result->encrypted_message = &encrypted_message;
    
}
