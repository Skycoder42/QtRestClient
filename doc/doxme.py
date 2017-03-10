#!/usr/bin/python
# $1 The readme to be transformed
# $pwd: dest dir

import sys

def readFirst(line, out):
	if line[0:2] != "# ":
		raise ValueError("Expected first line to start with '# '")
	# skip the first line
	out.write("[TOC]\n\n")

readCounter = 0
def readMore(line, out):
	global readCounter
	if line[0:2] == "##":
		out.write(line[1:] + " {{#readme_label_{}}}\n".format(readCounter))
		readCounter += 1
	else:
		out.write(line + "\n")

#read args
readme = sys.argv[1]
doxme = "./README.md"

inFile = open(readme, "r")
outFile = open(doxme, "w")

isFirst = True
for line in inFile:
	print(line.strip())
	if isFirst:
		readFirst(line.strip(), outFile)
		isFirst = False
	else:
		readMore(line.strip(), outFile)
		
inFile.close();
outFile.close();