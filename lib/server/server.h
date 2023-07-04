#ifndef SERVER_H
#define SERVER_H

typedef struct {
    unsigned char* publicKey;
    unsigned char* privateKey;
} KeyPair;

KeyPair generateKeyPair();
unsigned char* decryptMessage(const unsigned char* encryptedMessage, const unsigned char* privateKey, int encryptedSize, int* decryptedSize);

#endif
