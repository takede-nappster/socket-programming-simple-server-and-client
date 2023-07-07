all:
	gcc -o server demo_server.c lib/server/server.c -lcrypto
	gcc -o client demo_client.c lib/client/client.c -lcrypto
