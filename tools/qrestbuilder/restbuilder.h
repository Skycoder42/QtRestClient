#ifndef RESTBUILDER_H
#define RESTBUILDER_H

#include <QJsonObject>
#include <QObject>

class RestBuilder : public QObject
{
	Q_OBJECT
public:
	explicit RestBuilder(QObject *parent = nullptr);

	void buildClass(const QString &in, const QString &hOut, const QString &cppOut);
	void buildObject(const QString &in, const QString &hOut, const QString &cppOut);

private:
	QJsonObject readJson(const QString &fileName);

	void generateApiObject(const QString &name, const QJsonObject &obj);
	void generateApiGadget(const QString &name, const QJsonObject &obj);

};

#endif // RESTBUILDER_H
