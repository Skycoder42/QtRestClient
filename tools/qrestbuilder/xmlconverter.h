#ifndef XMLCONVERTER_H
#define XMLCONVERTER_H

#include <functional>
#include <QJsonObject>
#include <QObject>
#include <QXmlStreamWriter>

class XmlConverter : public QObject
{
	Q_OBJECT

public:
	explicit XmlConverter(QObject *parent = nullptr);

	void convert(const QString &type, const QString &in, const QString &out);

private:
	void writeObjectXml(const QJsonObject &data, QXmlStreamWriter &writer);
	void writeClassXml(const QJsonObject &data, QXmlStreamWriter &writer);

	void writeIncludes(const QJsonArray &includes, QXmlStreamWriter &writer);

	void writeAttrIf(const QJsonObject &data, QXmlStreamWriter &writer, const QString &baseKey, QString newKey = {});
	void writeAttrIfAny(const QJsonObject &data, QXmlStreamWriter &writer, const QString &baseKey);

	void writeExpr(const QJsonObject &data, QXmlStreamWriter &writer, const QString &baseKey, QString newKey = {}, const std::function<void(QXmlStreamWriter&)> &attrFn = {});
	void writeFixParams(const QJsonObject &data, QXmlStreamWriter &writer, const QString &baseKey, const QString &newKey);
	void writeParamList(const QJsonArray &params, QXmlStreamWriter &writer, const QString &newKey);
};

#endif // XMLCONVERTER_H
