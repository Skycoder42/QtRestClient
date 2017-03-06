#include "restbuilder.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>

RestBuilder::RestBuilder() :
	QObject()
{}

void RestBuilder::build(const QString &in, const QString &hOut, const QString &cppOut)
{
	inFile = in;
	root = readJson(in);

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

QJsonObject RestBuilder::readJson(const QString &fileName)
{
	QFile file(fileName);
	if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
		throwFile(file);

	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(file.readAll(), &error);
	if(error.error != QJsonParseError::NoError)
		throw QString(fileName + ": " + error.errorString());
	file.close();

	return doc.object();
}

void RestBuilder::throwFile(const QFile &file)
{
	throw QString(file.fileName() + ": " + file.errorString());
}

RestBuilder::TypeInfo RestBuilder::readType(const QString &type)
{
	static QRegularExpression regex(QStringLiteral(R"__(^([^\$]*)(?:(\$)([^\$]*))?$)__"),
									QRegularExpression::OptimizeOnFirstUsageOption);

	auto match = regex.match(type);
	TypeInfo res;
	if(match.hasMatch()) {
		res.name = match.captured(1);
		res.isPointer = res.name.endsWith("*");
		if(!match.captured(2).isEmpty()) {
			res.include = match.captured(3);
			if(res.include.isEmpty()) {
				res.include = res.name;
				res.include.replace('*', QString());
				res.include += ".h";
			}
		} else {
			if(res.name.at(0).isUpper()) {
				res.include = res.name;
				res.include.replace('*', QString());
			}
		}
	}

	return res;
}

void RestBuilder::writeIncGuardBegin()
{
	QString guard = inFile.baseName().toUpper() + "_H";
	header << "#ifndef "
		   << guard
		   << '\n'
		   << "#define "
		   << guard
		   << "\n\n";
}

void RestBuilder::writeIncGuardEnd()
{
	QString guard = inFile.baseName().toUpper() + "_H";
	header << "#endif //"
		   << guard
		   << '\n';
}

void RestBuilder::writeIncludes(QTextStream &stream, const QStringList &includes)
{
	foreach (auto inc, includes) {
		stream << "#include <"
			   << inc
			   << ">\n";
	}
	stream << '\n';
}

RestBuilder::TypeInfo::TypeInfo(const QString &name, bool isPointer, const QString &include) :
	name(name),
	isPointer(isPointer),
	include(include)
{}
