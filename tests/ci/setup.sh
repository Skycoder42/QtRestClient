#!/bin/bash

if [[ $PLATFORM == "gcc_64" ]]; then
	mv ./qtmodules-travis/ci/linux/setup-docker.sh ./qtmodules-travis/ci/linux/setup-docker-main.sh
	echo "#!/bin/bash" > ./qtmodules-travis/ci/linux/setup-docker.sh
	echo "apt-get update -qq" >> ./qtmodules-travis/ci/linux/setup-docker.sh
	echo "curl -sL https://deb.nodesource.com/setup_6.x | -E bash -" >> ./qtmodules-travis/ci/linux/setup-docker.sh
	echo "apt-get install -y nodejs" >> ./qtmodules-travis/ci/linux/setup-docker.sh
	echo "npm install -g json-server" >> ./qtmodules-travis/ci/linux/setup-docker.sh
	echo '$(dirname "$0")/setup-docker-main.sh' >> ./qtmodules-travis/ci/linux/setup-docker.sh
else
	if [[ $TRAVIS_OS_NAME == "linux" ]]; then
		sudo apt-get update -qq
		curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash -
		sudo apt-get install -y nodejs
	fi

	sudo npm install -g json-server
fi
