#!/bin/bash
openssl genrsa -out redis-server.key 4096

openssl req \
    -new -sha256 \
    -key redis-server.key \
    -config server.conf | \
    openssl x509 \
        -req -sha256 \
        -CA ca.crt \
        -CAkey ca.key \
        -CAserial ca.txt \
        -CAcreateserial \
        -days 365 \
        -out redis-server.crt
