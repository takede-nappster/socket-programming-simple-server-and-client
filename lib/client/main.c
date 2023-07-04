#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include "client.h"

int main() {
    Client client;
    client.public_key ="-----BEGIN RSA PUBLIC KEY-----\n"
                    "MIIBCgKCAQEA7UN0DoMCDPIT27/tKFpVMREC0uZFEwlc3VIAcTyrEF9c4P4UCeaA\n"
                    "mKzIKvp/vvAKdnUiL1WOEvqCVacIwCo/nDLLRnqKY1SQs8toy9HSWnWguFEXUPi8\n"
                    "GXnQuq7BklcSPKpD9rpgzEyVwj53Z2eLRJXx9Qaael9VLYD4PkIeEhs30Ga25J8D\n"
                    "5ST4Xl7vQZ+mFA2a8BpVXf2eHh5XagTIz+UsuFp2XHmY+Y1jBRR95Ea/zV6JbxPB\n"
                    "q95s8if5lTp/Nn+L4dCvJQMcknI02mmlJa5uM48UWMJPddZLsqOrOgIuCl2o451+\n"
                    "R+r1ZfM00l/rm6UtZZ6Vnn34O+n4Ts5I4QIDAQAB\n"
                    "-----END RSA PUBLIC KEY-----\n";


    char message[1024];
    printf("Entrez votre message: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = '\0';
    EncryptedResult result;
    encrypt_message(client, message, &result);

    printf("### size : %d \n", result.encrypted_length);
    for (int i = 0; i < result.encrypted_length; i++) {
        printf("%02x", *(result.encrypted_message)[i]);
    }

    return 0;
}
