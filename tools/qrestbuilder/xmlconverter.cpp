#include "xmlconverter.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QVariant>

XmlConverter::XmlConverter(QObject *parent) :
	QObject(parent)
{}

void XmlConverter::convert(const QString &type, const QString &in, const QString &out)
{
	QFile inFile(in);
	if(!inFile.open(QIODevice::ReadOnly | QIODevice::Text))
		throw tr("%1: %2").arg(inFile.fileName(), inFile.errorString());

	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(inFile.readAll(), &error);
	if(error.error != QJsonParseError::NoError)
		throw tr("%1: %2").arg(in, error.errorString());
	inFile.close();

	QFile outFile(out);
	if(!outFile.open(QIODevice::WriteOnly | QIODevice::Text))
		throw tr("%1: %2").arg(outFile.fileName(), outFile.errorString());
	QXmlStreamWriter writer(&outFile);
	writer.setAutoFormatting(true);
	writer.setAutoFormattingIndent(-1);
	writer.writeStartDocument();

	if(type == QStringLiteral("object"))
		writeObjectXml(doc.object(), writer);
	else if(type == QStringLiteral("class"))
		writeClassXml(doc.object(), writer);
	else
		throw tr("Invalid conversion input type: %1. Must be either \"object\" or \"class\"").arg(type);

	if(writer.hasError())
		throw tr("%1: %2").arg(outFile.fileName(), outFile.errorString());
	writer.writeEndDocument();
	outFile.close();
}

void XmlConverter::writeObjectXml(const QJsonObject &data, QXmlStreamWriter &writer)
{
	bool isObject;
	if(data[QStringLiteral("$type")] == QStringLiteral("object"))
		isObject = true;
	else if(data[QStringLiteral("$type")] == QStringLiteral("gadget"))
		isObject = false;
	else
		throw tr("Unknown type: %1").arg(data[QStringLiteral("$type")].toString());

	if(isObject)
		writer.writeStartElement(QStringLiteral("RestObject"));
	else
		writer.writeStartElement(QStringLiteral("RestGadget"));
	writer.writeDefaultNamespace(QStringLiteral("https://skycoder42.de/QtRestClient/qrestbuilder"));

	// copy normal attributes
	writeAttrIf(data, writer, QStringLiteral("$name"));
	writeAttrIf(data, writer, QStringLiteral("$parent"), QStringLiteral("base"));
	writeAttrIf(data, writer, QStringLiteral("$export"));
	writeAttrIf(data, writer, QStringLiteral("$registerConverters"));
	writeAttrIf(data, writer, QStringLiteral("$testEquality"));
	writeAttrIf(data, writer, QStringLiteral("$generateEquals"));

	// copy includes
	writeIncludes(data[QStringLiteral("$includes")].toArray(), writer);

	// copy enums
	auto jEnums = data[QStringLiteral("$enums")].toObject();
	for(auto it = jEnums.constBegin(); it != jEnums.constEnd(); it++) {
		writer.writeStartElement(QStringLiteral("Enum"));
		writer.writeAttribute(QStringLiteral("name"), it.key());
		QJsonArray values;
		if(it->isArray())
			values = it->toArray();
		else {
			auto eObj = it->toObject();
			writeAttrIfAny(eObj, writer, QStringLiteral("isFlags"));
			writeAttrIfAny(eObj, writer, QStringLiteral("base"));
			values = eObj[QStringLiteral("values")].toArray();
		}
		for(const auto value : values) {
			auto vStr = value.toString().split(QLatin1Char(':'));
			writer.writeStartElement(QStringLiteral("Key"));
			writer.writeAttribute(QStringLiteral("name"), vStr.takeFirst());
			if(!vStr.isEmpty())
				writer.writeCharacters(vStr.join(QLatin1Char(':')));
			writer.writeEndElement();
		}
		writer.writeEndElement();
	}

	// copy properties
	for(auto it = data.constBegin(); it != data.constEnd(); it++) {
		if(it.key().startsWith(QLatin1Char('$')))
			continue;

		writer.writeStartElement(QStringLiteral("Property"));
		writer.writeAttribute(QStringLiteral("key"), it.key());
		writer.writeAttribute(QStringLiteral("type"), it.value().toString());
		writer.writeEndElement();
	}

	writer.writeEndElement();
}

void XmlConverter::writeClassXml(const QJsonObject &data, QXmlStreamWriter &writer)
{
	bool isApi;
	if(data[QStringLiteral("type")] == QStringLiteral("api"))
		isApi = true;
	else if(data[QStringLiteral("type")] == QStringLiteral("class"))
		isApi = false;
	else
		throw tr("Unknown type: %1").arg(data[QStringLiteral("type")].toString());

	if(isApi)
		writer.writeStartElement(QStringLiteral("RestApi"));
	else
		writer.writeStartElement(QStringLiteral("RestClass"));
	writer.writeDefaultNamespace(QStringLiteral("https://skycoder42.de/QtRestClient/qrestbuilder"));

	// copy normal attributes
	writeAttrIfAny(data, writer, QStringLiteral("name"));
	writeAttrIfAny(data, writer, QStringLiteral("export"));
	writeAttrIf(data, writer, QStringLiteral("parent"), QStringLiteral("base"));
	writeAttrIfAny(data, writer, QStringLiteral("except"));
	// copy api attributes
	if(isApi) {
		writeAttrIfAny(data, writer, QStringLiteral("globalName"));
		writeAttrIfAny(data, writer, QStringLiteral("autoCreate"));
	}

	// copy includes
	writeIncludes(data[QStringLiteral("includes")].toArray(), writer);

	// specific stuff
	if(isApi) {
		writeExpr(data, writer, QStringLiteral("baseUrl"), {}, [&](QXmlStreamWriter &xWriter){
			writeAttrIfAny(data, xWriter, QStringLiteral("apiVersion"));
		});
		writeFixParams(data, writer, QStringLiteral("parameters"), QStringLiteral("Parameter"));
		writeFixParams(data, writer, QStringLiteral("headers"), QStringLiteral("Header"));
	} else
		writeExpr(data, writer, QStringLiteral("path"));

	// classes
	auto jClasses = data[QStringLiteral("classes")].toObject();
	for(auto it = jClasses.constBegin(); it != jClasses.constEnd(); it++) {
		writer.writeStartElement(QStringLiteral("Class"));
		writer.writeAttribute(QStringLiteral("key"), it.key());
		writer.writeAttribute(QStringLiteral("type"), it->toString());
		writer.writeEndElement();
	}

	// methods
	auto jMethods = data[QStringLiteral("methods")].toObject();
	for(auto it = jMethods.constBegin(); it != jMethods.constEnd(); it++) {
		auto method = it->toObject();
		writer.writeStartElement(QStringLiteral("Method"));
		writer.writeAttribute(QStringLiteral("name"), it.key());
		writeAttrIfAny(method, writer, QStringLiteral("verb"));
		writeAttrIfAny(method, writer, QStringLiteral("body"));
		writeAttrIfAny(method, writer, QStringLiteral("returns"));
		writeAttrIfAny(method, writer, QStringLiteral("except"));

		// path stuff (fixed + params)
		writeExpr(method, writer, QStringLiteral("url"));
		writeExpr(method, writer, QStringLiteral("path"));
		writeParamList(method[QStringLiteral("pathParams")].toArray(), writer, QStringLiteral("PathParam"));

		//params and header
		writeParamList(method[QStringLiteral("parameters")].toArray(), writer, QStringLiteral("Param"));
		writeFixParams(method, writer, QStringLiteral("headers"), QStringLiteral("Header"));

		writer.writeEndElement();
	}

	writer.writeEndElement();
}

void XmlConverter::writeIncludes(const QJsonArray &includes, QXmlStreamWriter &writer)
{
	for(const auto &value : includes) {
		auto inc = value.toString();
		if(inc.startsWith(QLatin1Char('>'))) {
			writer.writeStartElement(QStringLiteral("Include"));
			writer.writeAttribute(QStringLiteral("local"), QStringLiteral("true"));
			writer.writeCharacters(inc.mid(1));
			writer.writeEndElement();
		} else
			writer.writeTextElement(QStringLiteral("Include"), inc);
	}
}

void XmlConverter::writeAttrIf(const QJsonObject &data, QXmlStreamWriter &writer, const QString &baseKey, QString newKey)
{
	if(newKey.isNull())
		newKey = baseKey.mid(1);
	if(data.contains(baseKey))
		writer.writeAttribute(newKey, data.value(baseKey).toVariant().toString());
}

void XmlConverter::writeAttrIfAny(const QJsonObject &data, QXmlStreamWriter &writer, const QString &baseKey)
{
	writeAttrIf(data, writer, baseKey, baseKey);
}

void XmlConverter::writeExpr(const QJsonObject &data, QXmlStreamWriter &writer, const QString &baseKey, QString newKey, const std::function<void(QXmlStreamWriter&)> &attrFn)
{
	if(!data.contains(baseKey))
		return;
	if(newKey.isNull())
		newKey = baseKey.mid(0, 1).toUpper() + baseKey.mid(1);

	writer.writeStartElement(newKey);
	if(attrFn)
		attrFn(writer);
	auto val = data.value(baseKey).toString();
	if(val.startsWith(QLatin1Char('$'))) {
		writer.writeAttribute(QStringLiteral("expr"), QStringLiteral("true"));
		writer.writeCharacters(val.mid(1));
	} else
		writer.writeCharacters(val);
	writer.writeEndElement();
}

void XmlConverter::writeFixParams(const QJsonObject &data, QXmlStreamWriter &writer, const QString &baseKey, const QString &newKey)
{
	auto mData = data.value(baseKey).toObject();
	for(auto it = mData.constBegin(); it != mData.constEnd(); it++) {
		writer.writeStartElement(newKey);
		writer.writeAttribute(QStringLiteral("key"), it.key());

		auto val = it->toString();
		if(val.startsWith(QLatin1Char('$'))) {
			writer.writeAttribute(QStringLiteral("expr"), QStringLiteral("true"));
			writer.writeCharacters(val.mid(1));
		} else
			writer.writeCharacters(val);

		writer.writeEndElement();
	}
}

void XmlConverter::writeParamList(const QJsonArray &params, QXmlStreamWriter &writer, const QString &newKey)
{
	for(const auto &pValue : params) {
		auto param = pValue.toString().split(QLatin1Char(';'));
		if(param.size() < 2)
			continue;
		writer.writeStartElement(newKey);
		writer.writeAttribute(QStringLiteral("key"), param.takeFirst());
		writer.writeAttribute(QStringLiteral("type"), param.takeFirst());
		if(!param.isEmpty()) {
			writer.writeAttribute(QStringLiteral("asStr"), QStringLiteral("true"));
			writer.writeCharacters(param.join(QLatin1Char(';')));
		}
		writer.writeEndElement();
	}
}
