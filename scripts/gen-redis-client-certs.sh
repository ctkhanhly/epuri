#!/bin/bash
openssl genrsa -out redis-client.key 4096

openssl req \
    -new -sha256 \
    -key redis-client.key \
    -config client.conf | \
    openssl x509 \
        -req -sha256 \
        -CA ca.crt \
        -CAkey ca.key \
        -CAserial ca.txt \
        -CAcreateserial \
        -days 365 \
        -out redis-client.crt

openssl genrsa -out redis-client-secure.key 4096

openssl req \
    -new -sha256 \
    -key redis-client-secure.key \
    -config client-secure.conf | \
    openssl x509 \
        -req -sha256 \
        -CA ca.crt \
        -CAkey ca.key \
        -CAserial ca.txt \
        -CAcreateserial \
        -days 365 \
        -out redis-client-secure.crt

