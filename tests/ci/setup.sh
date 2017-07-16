#!/bin/bash
set -e

buildFileName=build.sh

if [[ $PLATFORM == "gcc_64" ]]; then
	mv ./qtmodules-travis/ci/linux/setup-docker.sh ./qtmodules-travis/ci/linux/setup-docker.sh.tmp
	mv $(dirname "$0")/setup-docker.sh ./qtmodules-travis/ci/linux/setup-docker.sh
	cat ./qtmodules-travis/ci/linux/setup-docker.sh.tmp >> ./qtmodules-travis/ci/linux/setup-docker.sh

	buildFileName=build-all.sh
else
	if [[ $TRAVIS_OS_NAME == "linux" ]]; then
		curl -sL https://deb.nodesource.com/setup_6.x | sudo -E bash -
		sudo apt-get install -y nodejs

		buildFileName=build-all.sh
	fi

	sudo npm install -g json-server
fi

mv ./qtmodules-travis/ci/$TRAVIS_OS_NAME/$buildFileName ./qtmodules-travis/ci/$TRAVIS_OS_NAME/$buildFileName.tmp
echo "#!/bin/bash" > ./qtmodules-travis/ci/$TRAVIS_OS_NAME/$buildFileName
echo ./tests/auto/restclient/rest-db-setup.sh >> ./qtmodules-travis/ci/$TRAVIS_OS_NAME/$buildFileName
cat ./qtmodules-travis/ci/$TRAVIS_OS_NAME/$buildFileName.tmp >> ./qtmodules-travis/ci/$TRAVIS_OS_NAME/$buildFileName
chmod a+x ./qtmodules-travis/ci/$TRAVIS_OS_NAME/$buildFileName

# disable test on osx as workaround
if [[ $PLATFORM == "clang_64" ]]; then
	echo "SUBDIRS -= RestReplyTest" >> ./tests/auto/restclient/restclient.pro
fi
