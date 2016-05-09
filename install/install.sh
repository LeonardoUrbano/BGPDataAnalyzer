#!/bin/bash
# Install script

# openssl
sudo apt-get install openssl
# libcurl
sudo apt-get install multiarch-support
sudo apt-get install ca-certificates
sudo apt-get install libc6 
sudo apt-get install libgssapi-krb5-2
sudo apt-get install libidn11
sudo apt-get install libldap-2.4-2
sudo apt-get install librtmp0
sudo apt-get install libssl1.0.0
sudo apt-get install zlib1g
sudo apt-get install libkrb5-dev
sudo apt-get install libldap2-dev librtmp-dev libssl-dev zlib1g-dev
sudo apt-get install librtmp-dev libssl-dev zlib1g-dev
sudo apt-get install python-dev
sudo apt-get install libbz2-dev
sudo apt-get install libcurl4-openssl-dev
# lboost
sudo apt-get install libboost-all-dev
# websocketpp
sudo dpkg -i "packages/libwebsocketpp-dev_0.6.0-1_all.deb"
sudo dpkg -i "packages/libwebsocketpp-doc_0.6.0-1_all.deb"
