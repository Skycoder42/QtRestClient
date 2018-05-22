#include "restbuilder.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RestBuilder::RestBuilder(QXmlStreamReader &inStream) :
	reader(inStream)
{}

RestBuilder::~RestBuilder() = default;

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

	QSaveFile headerFile(hOut);
	if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(headerFile);
	header.setDevice(&headerFile);

	QSaveFile sourceFile(cppOut);
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
			throwReader(QStringLiteral("Value of attribute \"%1\" is not a xs:boolean!").arg(key));
	} else if(required)
		throwReader(QStringLiteral("Required attribute \"%1\" but was not set").arg(key));
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
	throw QStringLiteral("%1: %2").arg(file.fileName(), file.errorString());
}

void RestBuilder::throwReader(const QString &overwriteError) const
{
	throwReader(reader, overwriteError);
}

void RestBuilder::throwReader(QXmlStreamReader &stream, const QString &overwriteError)
{
	throw QStringLiteral("%1:%2:%3: %4")
			.arg(dynamic_cast<QFileDevice*>(stream.device())->fileName())
			.arg(stream.lineNumber())
			.arg(stream.columnNumber())
			.arg(overwriteError.isNull() ? stream.errorString() : overwriteError);
}

void RestBuilder::throwChild()
{
	throwReader(QStringLiteral("Unexpected child element: %1").arg(reader.name()));
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

QString RestBuilder::nsName(const QString &name, const QString &nspace) const
{
	if(nspace.isEmpty())
		return name;
	else
		return nspace + QStringLiteral("::") + name;
}

QString RestBuilder::nsInject(const QString &name, const QString &prefix) const
{
	auto nList = name.split(QStringLiteral("::"));
	nList.last().prepend(prefix);
	return nList.join(QStringLiteral("::"));
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
	if(param.asStr) {
		if(param.type == QStringLiteral("QString"))
			return QStringLiteral("QStringLiteral(\"") + param.defaultValue + QStringLiteral("\")");
		else if(param.type == QStringLiteral("QByteArray"))
			return QStringLiteral("QByteArrayLiteral(\"") + param.defaultValue + QStringLiteral("\")");
		else
			return QStringLiteral("QVariant(QStringLiteral(\"") + param.defaultValue + QStringLiteral("\")).value<") + param.type + QStringLiteral(">()");
	} else
		return param.defaultValue;
}

QString RestBuilder::writeParamArg(const RestBuilder::BaseParam &param, bool withDefault)
{
	QString res = QStringLiteral("const ") + param.type + QStringLiteral(" &") + param.key;
	if(withDefault && !param.defaultValue.isEmpty())
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
