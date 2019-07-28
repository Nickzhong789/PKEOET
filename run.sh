gcc -o PKEOET PKEOET.c -L /usr/lib -lssl -lcrypto -ldl -lpthread -L ~/Workspace/Crypt/lib/openssl-1.0.1h -L ~/Workspace/Crypt/lib/openssl-1.0.1h/include -lpbc -L /usr/include/pbc -L /usr/lib -lgmp -L /usr/include -L /usr/lib

./PKEOET < params/e.param


