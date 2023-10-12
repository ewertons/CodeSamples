openssl req -x509 -nodes -days 365 -newkey rsa:2048 -keyout mycert.pem -out mycert.pem -subj "/C=US/ST=WA/L=Seattle/O=Contoso/OU=Some Unit"
gcc -Wall -o server server.c -L/usr/lib -lssl -lcrypto
gcc -Wall -o client client.c -L/usr/lib -lssl -lcrypto

