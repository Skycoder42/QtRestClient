#!/bin/bash
set -ex

apt-get -qq update
apt-get -qq install --no-install-recommends curl
curl -Lo /tmp/instnode.sh https://deb.nodesource.com/setup_6.x
chmod a+x /tmp/instnode.sh
/tmp/instnode.sh
apt-get install -y nodejs
which nodejs
which npm
npm install -g json-server
