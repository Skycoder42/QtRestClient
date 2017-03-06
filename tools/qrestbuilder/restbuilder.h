#ifndef RESTBUILDER_H
#define RESTBUILDER_H

#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonObject>
#include <QTextStream>

class RestBuilder : public QObject
{
public:
	explicit RestBuilder();

	void build(const QString &in, const QString &hOut, const QString &cppOut);

protected:
	virtual void build(const QFileInfo &inFile) = 0;

	QJsonObject readJson(const QString &fileName);
	void throwFile(const QFile &file);

	QJsonObject root;
	QTextStream header;
	QTextStream source;
};

#endif // RESTBUILDER_H
