#!/bin/bash
set -ex

apt-get -qq update
apt-get -qq install --no-install-recommends curl
curl -sL https://deb.nodesource.com/setup_6.x | bash -
apt-get install -y nodejs
which nodejs
which npm
npm install -g json-server
