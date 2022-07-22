#!/bin/bash
mkdir -p tls
openssl genrsa -out tls/ca.key 4096
openssl req \
    -x509 -new -nodes -sha256 \
    -key tls/ca.key \
    -days 3650 \
    -subj '/O=Redis Test/CN=Certificate Authority' \
    -out tls/ca.crt

openssl dhparam -out tls/redis.dh 2048