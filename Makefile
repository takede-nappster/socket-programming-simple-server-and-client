all:
	gcc -o server server.c lib/server/server.c -lcrypto
	gcc -o client client.c lib/client/client.c -lcrypto
