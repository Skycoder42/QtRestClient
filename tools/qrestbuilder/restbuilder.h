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
	struct TypeInfo {
		QString name;
		bool isPointer;
		QString include;

		TypeInfo(const QString &name = {}, bool isPointer = false, const QString &include = {});
	};

	virtual void build() = 0;

	QJsonObject readJson(const QString &fileName);
	void throwFile(const QFile &file);

	TypeInfo readType(const QString &type);
	void writeIncludes(QTextStream &stream, const QStringList &includes);

	QFileInfo inFile;
	QJsonObject root;
	QTextStream header;
	QTextStream source;

private:
	void writeIncGuardBegin();
	void writeIncGuardEnd();
};

#endif // RESTBUILDER_H
