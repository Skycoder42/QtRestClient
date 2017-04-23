#!/bin/bash
# IMPORTANT: Adjust path to script of https://github.com/Skycoder42/QtModules (repogen.py)
# $1 path to module binaries
# $2 Version

myDir=$(dirname "$0")
"$myDir/../QtModules/repogen.py" "$1" RestClient "qt.58.skycoder42.jsonserializer" "qrestbuilder" "A library for generic JSON-based REST-APIs, with a mechanism to map JSON to Qt objects." "$2" "$myDir/LICENSE" BSD-3-Clause
