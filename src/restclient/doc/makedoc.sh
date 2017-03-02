#!/bin/bash
# $1: $$SRCDIR
# $2: $$VERSION
# $3: $$[QT_INSTALL_BINS]
# $4: $$[QT_INSTALL_HEADERS]
# $5: $$[QT_INSTALL_DOCS]
# $pwd: dest dir

destDir="$(pwd)/../../doc"
srcDir="$1/doc"
version=$2
verTag=$(echo "$version" | sed -e 's/\.//g')
qtBins=$3
qtHeaders=$4
qtDocs=$5
doxyTmp="$destDir/Doxyfile"

cd $srcDir

mkdir $destDir
cat Doxyfile > "$doxyTmp"
echo "PROJECT_NUMBER = \"$version\"" >> "$doxyTmp"
echo "OUTPUT_DIRECTORY = \"$destDir\"" >> "$doxyTmp"
echo "QHP_NAMESPACE = \"de.skycoder42.qtrestclient.$verTag\"" >> "$doxyTmp"
echo "QHP_CUST_FILTER_NAME = \"RestClient $version\"" >> "$doxyTmp"
echo "QHP_CUST_FILTER_ATTRS = \"qtrestclient $version\"" >> "$doxyTmp"
echo "QHG_LOCATION = \"$qtBins/qhelpgenerator\"" >> "$doxyTmp"
echo "INCLUDE_PATH += \"$qtHeaders\"" >> "$doxyTmp"
echo "GENERATE_TAGFILE = \"$destDir/qtrestclient/qtrestclient.tags\"" >> "$doxyTmp"
if [ "$DOXY_STYLE" ]; then
	echo "HTML_STYLESHEET = \"$DOXY_STYLE\"" >> "$doxyTmp"
fi
if [ "$DOXY_STYLE_EXTRA" ]; then
	echo "HTML_EXTRA_STYLESHEET = \"$DOXY_STYLE_EXTRA\"" >> "$doxyTmp"
fi

for tagFile in $(find "$qtDocs" -name *.tags); do
	if [ $(basename "$tagFile") !=  "qtjsonserializer.tags" ]; then
		echo "TAGFILES += \"$tagFile=https://doc.qt.io/qt-5\"" >> "$doxyTmp"
	else
		echo "SKIPPED $tagFile!"
	fi
done

doxygen "$doxyTmp"
