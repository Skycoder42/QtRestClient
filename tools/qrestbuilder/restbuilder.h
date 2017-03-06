#ifndef RESTBUILDER_H
#define RESTBUILDER_H

#include <QFile>
#include <QIODevice>
#include <QJsonObject>
#include <QObject>
#include <QTextStream>

class RestBuilder : public QObject
{
	Q_OBJECT
public:
	explicit RestBuilder(QObject *parent = nullptr);

	void buildClass(const QString &in, const QString &hOut, const QString &cppOut);
	void buildObject(const QString &in, const QString &hOut, const QString &cppOut);

private:
	QJsonObject readJson(const QString &fileName);

	void generateApiObject(const QString &name, const QJsonObject &obj, QTextStream &header, QTextStream &source);
	void generateApiGadget(const QString &name, const QJsonObject &obj, QTextStream &header, QTextStream &source);

	void throwFile(const QFile &file);

};

#endif // RESTBUILDER_H
