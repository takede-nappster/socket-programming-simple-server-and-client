#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "server.h"

KeyPair generateKeyPair() {
    KeyPair keyPair;
    RSA* rsaKeyPair = RSA_new();
    BIGNUM* bne = BN_new();
    unsigned long e = RSA_F4;

    if (BN_set_word(bne, e) != 1) {
        fprintf(stderr, "Erreur lors de la génération de la clé RSA (BN_set_word).\n");
        exit(1);
    }

    if (RSA_generate_key_ex(rsaKeyPair, 2048, bne, NULL) != 1) {
        fprintf(stderr, "Erreur lors de la génération de la clé RSA (RSA_generate_key_ex).\n");
        exit(1);
    }

    BIO* publicKeyBio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_RSAPublicKey(publicKeyBio, rsaKeyPair) != 1) {
        fprintf(stderr, "Erreur lors de l'écriture de la clé publique (PEM_write_bio_RSAPublicKey).\n");
        exit(1);
    }

    BIO* privateKeyBio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_RSAPrivateKey(privateKeyBio, rsaKeyPair, NULL, NULL, 0, NULL, NULL) != 1) {
        fprintf(stderr, "Erreur lors de l'écriture de la clé privée (PEM_write_bio_RSAPrivateKey).\n");
        exit(1);
    }

    keyPair.publicKey = (unsigned char*)malloc(2048);
    keyPair.privateKey = (unsigned char*)malloc(2048);

    int publicKeyLen = BIO_read(publicKeyBio, keyPair.publicKey, 2048);
    keyPair.publicKey[publicKeyLen] = '\0';

    int privateKeyLen = BIO_read(privateKeyBio, keyPair.privateKey, 2048);
    keyPair.privateKey[privateKeyLen] = '\0';

    RSA_free(rsaKeyPair);
    BIO_free(publicKeyBio);
    BIO_free(privateKeyBio);
    BN_free(bne);

    return keyPair;
}

unsigned char* decryptMessage(const unsigned char* encryptedMessage, const unsigned char* privateKey, int encryptedSize, int* decryptedSize) {
    RSA* rsa = RSA_new();
    BIO* privateKeyBio = BIO_new(BIO_s_mem());
    BIO_write(privateKeyBio, privateKey, strlen((char*)privateKey));
    PEM_read_bio_RSAPrivateKey(privateKeyBio, &rsa, NULL, NULL);

    *decryptedSize = RSA_size(rsa);
    unsigned char* decryptedMessage = (unsigned char*)malloc(*decryptedSize);
    memset(decryptedMessage, 0, *decryptedSize);

    int result = RSA_private_decrypt(encryptedSize, encryptedMessage, decryptedMessage, rsa, RSA_PKCS1_PADDING);

    if (result == -1) {
        fprintf(stderr, "Erreur lors du déchiffrement du message (RSA_private_decrypt).\n");
        ERR_print_errors_fp(stderr);
        exit(1);
    }

    RSA_free(rsa);
    BIO_free(privateKeyBio);

    return decryptedMessage;
}
