#ifndef CLIENT_H
#define CLIENT_H

typedef struct {
    char *public_key;
} Client;

typedef struct 
{
    int encrypted_length;
    char * * encrypted_message;
} EncryptedResult;


void encrypt_message(Client client, const char *message, EncryptedResult * result);

#endif /* CLIENT_H */
