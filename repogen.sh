#!/bin/bash
# IMPORTANT: Adjust path to script of https://github.com/Skycoder42/QtModules (repogen.py)
# $1 path to module binaries
# $2 Version

myDir=$(dirname "$0")
qtDir=${1?First parameter must be set to the dir to install}
version=${2?Set the version as second parameter}
qtvid=$(echo $qtDir | sed -e "s/\\.//g")

"$myDir/../QtModules/deploy/repogen.py"  "$qtDir" RestClient "qt.qt5.$qtvid.skycoder42.jsonserializer" "qrestbuilder" "A library for generic JSON-based REST-APIs, with a mechanism to map JSON to Qt objects." "$version" "$myDir/LICENSE" BSD-3-Clause
