#ifndef RESTBUILDER_H
#define RESTBUILDER_H

#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QJsonObject>
#include <QTextStream>

class RestBuilder : public QObject
{
	Q_OBJECT

public:
	explicit RestBuilder();

	void build(const QString &in, const QString &hOut, const QString &cppOut);

protected:
	virtual void build() = 0;
	virtual QString specialPrefix() = 0;

	QJsonObject readJson(const QString &fileName);
	void throwFile(const QFile &file);

	QStringList readIncludes();
	void writeIncludes(QTextStream &stream, const QStringList &includes);

	QString fileName;
	QString className;
	QString exportedClassName;
	QJsonObject root;
	QTextStream header;
	QTextStream source;

private:
	void writeIncGuardBegin();
	void writeIncGuardEnd();
};

#endif // RESTBUILDER_H
