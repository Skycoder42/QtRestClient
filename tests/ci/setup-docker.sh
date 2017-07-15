#!/bin/bash

apt-get -qq update
apt-get -qq install --no-install-recommends curl
curl -sL https://deb.nodesource.com/setup_6.x | bash -
apt-get install -y nodejs
npm install -g json-server

$(dirname "$0")/setup-docker-main.sh
