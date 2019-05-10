#!/bin/sh

#Chiave Privata Server
openssl genrsa -out keys/server_privkey.pem 3072

#Chiave Pubblica Server
openssl rsa -pubout -in keys/server_privkey.pem -out keys/server_pubkey.pem

#Chiave Privata Client
openssl genrsa -out keys/client_privkey.pem 3072

#Chiave Pubblica Client
openssl rsa -pubout -in keys/client_privkey.pem -out keys/client_pubkey.pem