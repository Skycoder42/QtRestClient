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
bool RestBuilder::readAttrib<bool>(const QString &key, const bool &defaultValue, bool required) const
{
	if(reader.attributes().hasAttribute(key)) {
		if(reader.attributes().value(key) == QStringLiteral("true"))
			return true;
		else if(reader.attributes().value(key) == QStringLiteral("false"))
			return false;
		else
			throwReader(tr("Value of attribute \"%1\" is not a xs:boolean!").arg(key));
	} else if(required)
		throwReader(tr("Required attribute \"%1\" but was not set").arg(key));
	else
		return defaultValue;
}

RestBuilder::Include RestBuilder::readInclude()
{
	auto local = readAttrib<bool>(QStringLiteral("local"), false);
	auto include = reader.readElementText();
	checkError();
	return {local, include};
}

RestBuilder::BaseParam RestBuilder::readBaseParam()
{
	BaseParam param;
	param.key = readAttrib(QStringLiteral("key"), {}, true);
	param.type = readAttrib(QStringLiteral("type"), {}, true);
	param.asStr = readAttrib<bool>(QStringLiteral("asStr"), false);
	param.defaultValue = reader.readElementText();
	checkError();
	return param;
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

QString RestBuilder::exportedName(const QString &name, const QString &exportKey) const
{
	if(exportKey.isEmpty())
		return name;
	else
		return exportKey + QLatin1Char(' ') + name;
}

void RestBuilder::writeIncludes(const QList<Include> &includes)
{
	for(const auto &inc : includes) {
		if(inc.local)
			header << "#include \"" << inc.include << "\"\n";
		else
			header << "#include <" << inc.include << ">\n";
	}
	header << "\n";
}

QString RestBuilder::writeParamDefault(const BaseParam &param)
{
	if(param.asStr)
		return QStringLiteral("QVariant(QStringLiteral(\"") + param.defaultValue + QStringLiteral("\")).value<") + param.type + QStringLiteral(">()");
	else
		return param.defaultValue;
}

QString RestBuilder::writeParamArg(const RestBuilder::BaseParam &param)
{
	QString res = param.type + QLatin1Char(' ') + param.key;
	if(!param.defaultValue.isEmpty())
		res += QStringLiteral(" = ") + writeParamDefault(param);
	return res;
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
