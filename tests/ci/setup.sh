#!/bin/bash
set -e

# disable test on osx as workaround
if [[ $PLATFORM == "clang_64" ]]; then
	echo "SUBDIRS -= RestReplyTest" >> ./tests/auto/restclient/restclient.pro
fi
