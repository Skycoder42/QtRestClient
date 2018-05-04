#include "restbuilder.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

RestBuilder::RestBuilder(QObject *parent) :
	QObject(parent)
{}

void RestBuilder::build(const QString &in, const QString &hOut, const QString &cppOut)
{
	root = readJson(in);

	QFileInfo inInfo(in);
	fileName = inInfo.baseName();
	className = root[specialPrefix() + QStringLiteral("name")].toString(fileName);
	exportedClassName = root[specialPrefix() + QStringLiteral("export")].toString();
	if(exportedClassName.isEmpty())
		exportedClassName = className;
	else
		exportedClassName += QLatin1Char(' ') + className;

	auto headerFile = new QFile(hOut, this);
	if(!headerFile->open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(*headerFile);
	header.setDevice(headerFile);

	auto sourceFile = new QFile(cppOut, this);
	if(!sourceFile->open(QIODevice::WriteOnly | QIODevice::Text))
		throwFile(*sourceFile);
	source.setDevice(sourceFile);

	writeIncGuardBegin();
	build();
	writeIncGuardEnd();

	header.flush();
	header.device()->close();
	source.flush();
	source.device()->close();
}

QJsonObject RestBuilder::readJson(const QString &filePath)
{
	QFile file(filePath);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		throwFile(file);

	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(file.readAll(), &error);
	if(error.error != QJsonParseError::NoError)
		throw tr("%1: %2").arg(filePath, error.errorString());
	file.close();

	return doc.object();
}

void RestBuilder::throwFile(const QFile &file)
{
	throw tr("%1: %2").arg(file.fileName(), file.errorString());
}

QStringList RestBuilder::readIncludes()
{
	QStringList res;
	auto incArray = root[specialPrefix() + QStringLiteral("includes")].toArray();
	res.reserve(incArray.size());
	for(auto include : incArray)
		res.append(include.toString());
	return res;
}

void RestBuilder::writeIncGuardBegin()
{
	QString guard = fileName.toUpper() + QStringLiteral("_H");
	header << "#ifndef "
		   << guard
		   << '\n'
		   << "#define "
		   << guard
		   << "\n\n";
}

void RestBuilder::writeIncGuardEnd()
{
	QString guard = fileName.toUpper() + QStringLiteral("_H");
	header << "#endif //"
		   << guard
		   << '\n';
}

void RestBuilder::writeIncludes(QTextStream &stream, const QStringList &includes)
{
	for(const auto &inc : QSet<QString>::fromList(includes)) {
		if(inc.startsWith(QLatin1Char('>'))) {
			stream << "#include \""
				   << inc.mid(1)
				   << "\"\n";
		} else {
			stream << "#include <"
				   << inc
				   << ">\n";
		}
	}
	stream << '\n';
}
