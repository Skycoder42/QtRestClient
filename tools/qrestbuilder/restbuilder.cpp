#include "restbuilder.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSaveFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RestBuilder::~RestBuilder() = default;

void RestBuilder::build(const QString &in, const QString &hOut, const QString &cppOut)
{
	fileName = QFileInfo{in}.baseName();

	QSaveFile headerFile(hOut);
	if(!headerFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw RestBuilderXmlReader::FileException(headerFile);
	header.setDevice(&headerFile);

	QSaveFile sourceFile(cppOut);
	if(!sourceFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw RestBuilderXmlReader::FileException(sourceFile);
	source.setDevice(&sourceFile);

	try {
		writeIncGuardBegin();
		build();
		writeIncGuardEnd();

		header.flush();
		if(!headerFile.commit())
			throw RestBuilderXmlReader::FileException(headerFile);
		source.flush();
		if(!sourceFile.commit())
			throw RestBuilderXmlReader::FileException(sourceFile);
	} catch(...) {
		headerFile.cancelWriting();
		sourceFile.cancelWriting();
		throw;
	}
}

QString RestBuilder::exportedName(const QString &name, const RestBuilderXmlReader::optional<QString> &exportKey) const
{
	if(exportKey)
		return exportKey.value() + QLatin1Char(' ') + name;
	else
		return name;
}

QString RestBuilder::nsName(const QString &name, const RestBuilderXmlReader::optional<QString> &nspace) const
{
	if(nspace)
		return nspace.value() + QStringLiteral("::") + name;
	else
		return name;
}

QString RestBuilder::nsInject(const QString &name, const QString &prefix) const
{
	auto nList = name.split(QStringLiteral("::"));
	nList.last().prepend(prefix);
	return nList.join(QStringLiteral("::"));
}

QString RestBuilder::boolValue(bool value) const
{
	return value ? QStringLiteral("true") : QStringLiteral("false");
}

void RestBuilder::writeIncludes(const QList<RestBuilderXmlReader::Include> &includes)
{
	for(const auto &inc : includes) {
		if(inc.local)
			header << "#include \"" << inc.include << "\"\n";
		else
			header << "#include <" << inc.include << ">\n";
	}
	header << "\n";
}

QString RestBuilder::writeParamDefault(const RestBuilderXmlReader::BaseParam &param)
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

QString RestBuilder::writeParamArg(const RestBuilderXmlReader::BaseParam &param, bool withDefault)
{
	QString res = QStringLiteral("const ") + param.type + QStringLiteral(" &") + param.key;
	if(withDefault && !param.defaultValue.isEmpty())
		res += QStringLiteral(" = ") + writeParamDefault(param);
	return res;
}

QString RestBuilder::writeExpression(const RestBuilderXmlReader::Expression &expression, bool asString)
{
	if(expression.expr)
		return expression.value;
	else if(asString)
		return QStringLiteral("QStringLiteral(\"") + expression.value + QStringLiteral("\")");
	else
		return QLatin1Char('"') + expression.value + QLatin1Char('"');
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
