#!/usr/bin/python
# $1 The readme to be transformed
# $pwd: dest dir

import sys
import re

def readFirst(line, out):
	if line[0:2] != "# ":
		raise ValueError("Expected first line to start with '# '")
	# skip the first line
	out.write("[TOC]\n\n")

readCounter = 0
regexp = re.compile(r'\W')
skip = False
def readMore(line, out):
	global skip
	if skip:
		if line[0:2] == "##":
			skip = False
		else:
			return

	if line.strip() == "## Table of contents":
		skip = True
	elif line[0:2] == "##":
		out.write(line[1:] + " {#" + regexp.sub('-', line.lstrip('#').strip()).lower() + "}\n")
	else:
		out.write(line + "\n")

#read args
readme = sys.argv[1]
doxme = "./README.md"

inFile = open(readme, "r")
outFile = open(doxme, "w")

isFirst = True
for line in inFile:
	if isFirst:
		readFirst(line[:-1], outFile)
		isFirst = False
	else:
		readMore(line[:-1], outFile)

inFile.close();
outFile.close();
