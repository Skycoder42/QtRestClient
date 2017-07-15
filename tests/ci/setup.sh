#!/bin/bash
set -e

if [[ $PLATFORM == "gcc_64" ]]; then
	mv ./qtmodules-travis/ci/linux/setup-docker.sh ./qtmodules-travis/ci/linux/setup-docker.sh.tmp
	mv $(dirname "$0")/setup-docker.sh ./qtmodules-travis/ci/linux/setup-docker.sh
	cat ./qtmodules-travis/ci/linux/setup-docker.sh.tmp >> ./qtmodules-travis/ci/linux/setup-docker.sh
else
	if [[ $TRAVIS_OS_NAME == "linux" ]]; then
		curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash -
		sudo apt-get install -y nodejs
	fi

	sudo npm install -g json-server
fi
