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

	build();

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

QPair<QString, QString> RestBuilder::splitType(const QString &type)
{
	static QRegularExpression regex(QStringLiteral(R"__(^([^\$]*)(?:(\$)([^\$]*))?$)__"),
									QRegularExpression::OptimizeOnFirstUsageOption);

	auto match = regex.match(type);
	QPair<QString, QString> res;
	if(match.hasMatch()) {
		res.first = match.captured(1);
		if(!match.captured(2).isEmpty()) {
			res.second = match.captured(3);
			if(res.second.isEmpty()) {
				res.second = res.first;
				res.second.replace(QLatin1Char('*'), QString());
				res.second += ".h";
			}
		} else {
			if(res.first.at(0).isUpper()) {
				res.second = res.first;
				res.second.replace(QLatin1Char('*'), QString());
			}
		}
	}

	return res;
}

void RestBuilder::writeIncGuardBegin()
{
	QString guard = inFile.baseName().toUpper() + "_H";
	header << QLatin1String("#ifndef ")
		   << guard
		   << QLatin1Char('\n')
		   << QLatin1String("#define ")
		   << guard
		   << QLatin1String("\n\n");
}

void RestBuilder::writeIncGuardEnd()
{
	QString guard = inFile.baseName().toUpper() + "_H";
	header << QLatin1String("#endif //")
		   << guard
		   << QLatin1Char('\n');
}

void RestBuilder::writeInclude(QTextStream &stream, const QStringList &includes)
{
	foreach (auto inc, includes) {
		stream << QLatin1String("#include <")
			   << inc
			   << QLatin1String(">\n");
	}
	stream << QLatin1Char('\n');
}
