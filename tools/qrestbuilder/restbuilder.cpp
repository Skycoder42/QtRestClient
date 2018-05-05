#include "restbuilder.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RestBuilder::RestBuilder(QXmlStreamReader &inStream, QObject *parent) :
	QObject(parent),
	reader(inStream)
{}

QString RestBuilder::readType(QXmlStreamReader &inStream)
{
	if(!inStream.readNextStartElement())
		throwReader(inStream);
	return inStream.name().toString();
}

void RestBuilder::build(const QString &in, const QString &hOut, const QString &cppOut)
{
	QFileInfo inInfo(in);
	fileName = inInfo.baseName();
	className = reader.attributes().value(QStringLiteral("name")).toString();
	exportedClassName = reader.attributes().value(QStringLiteral("export")).toString();
	if(exportedClassName.isEmpty())
		exportedClassName = className;
	else
		exportedClassName += QLatin1Char(' ') + className;

	QSaveFile headerFile(hOut, this);
	if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(headerFile);
	header.setDevice(&headerFile);

	QSaveFile sourceFile(cppOut, this);
	if(!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(sourceFile);
	source.setDevice(&sourceFile);

	try {
		writeIncGuardBegin();
		build();
		writeIncGuardEnd();

		header.flush();
		if(!headerFile.commit())
			throwFile(headerFile);
		source.flush();
		if(!sourceFile.commit())
			throwFile(sourceFile);
	} catch(...) {
		headerFile.cancelWriting();
		sourceFile.cancelWriting();
		throw;
	}
}

template<>
bool RestBuilder::readAttrib<bool>(const QString &key, const bool &defaultValue) const
{
	if(reader.attributes().hasAttribute(key)) {
		if(reader.attributes().value(key) == QStringLiteral("true"))
			return true;
		else if(reader.attributes().value(key) == QStringLiteral("false"))
			return false;
		else
			throwReader(tr("Value of attribute \"%1\" is not a xs:boolean!").arg(key));
	} else
		return defaultValue;
}

void RestBuilder::throwFile(const QFileDevice &file) const
{
	throw tr("%1: %2").arg(file.fileName(), file.errorString());
}

void RestBuilder::throwReader(const QString &overwriteError) const
{
	throwReader(reader, overwriteError);
}

void RestBuilder::throwReader(QXmlStreamReader &stream, const QString &overwriteError)
{
	throw tr("%1:%2:%3: %4")
			.arg(qobject_cast<QFileDevice*>(stream.device())->fileName())
			.arg(stream.lineNumber())
			.arg(stream.columnNumber())
			.arg(overwriteError.isNull() ? stream.errorString() : overwriteError);
}

void RestBuilder::throwChild()
{
	throwReader(tr("Unexpected child element: %1").arg(reader.name()));
}

void RestBuilder::checkError()
{
	if(reader.hasError())
		throwReader();
}

void RestBuilder::transformIncludes(const QStringList &extras)
{
	for(const auto &inc : extras)
		header << "#include <" << inc << ">\n";

	while(reader.readNextStartElement()) {
		checkError();

		if(reader.name() != QStringLiteral("Include"))
			break;
		auto local = readAttrib<bool>(QStringLiteral("local"), false);
		auto include = reader.readElementText();
		checkError();

		if(!extras.contains(include)) {
			if(local)
				header << "#include \"" << include << "\"\n";
			else
				header << "#include <" << include << ">\n";
		}

		if(reader.readNextStartElement())
			throwChild();
	}

	checkError();
	header << "\n";
}

void RestBuilder::writeIncGuardBegin()
{
	QString guard = fileName.toUpper() + QStringLiteral("_H");
	header << "#ifndef " << guard << '\n'
		   << "#define " << guard << "\n\n";
}

void RestBuilder::writeIncGuardEnd()
{
	QString guard = fileName.toUpper() + QStringLiteral("_H");
	header << "#endif //" << guard << '\n';
}
