#!/bin/bash

curl -sL https://deb.nodesource.com/setup_6.x | -E bash -
apt-get install -y nodejs
npm install -g json-server

$(dirname "$0")/setup-docker-main.sh
