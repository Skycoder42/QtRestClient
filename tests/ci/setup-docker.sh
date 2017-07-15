#!/bin/bash
set -e

apt-get -qq update
apt-get -qq install --no-install-recommends curl ca-certificates
curl -sL https://deb.nodesource.com/setup_6.x | bash -
apt-get install -y nodejs
npm install -g json-server

stat /usr/bin/json-server
echo $PATH
