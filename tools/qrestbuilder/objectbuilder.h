#ifndef OBJECTBUILDER_H
#define OBJECTBUILDER_H

#include "restbuilder.h"

class ObjectBuilder : public RestBuilder
{
	Q_OBJECT

public:
	explicit ObjectBuilder(QXmlStreamReader &inStream, QObject *parent = nullptr);

	static bool canReadType(const QString &type);

private:
	struct XmlContent {
		bool isObject = false;
		QString name;
		QString base;
		QString exportKey;
		QString nspace;
		bool registerConverters = false;
		bool testEquality = false;
		bool generateEquals = false;
		bool generateReset = false;
		QString simpleHref;

		QList<Include> includes;

		struct Enum {
			QString name;
			QString base;
			bool isFlags = false;

			struct Key {
				QString key;
				QString value;
			};
			QList<Key> keys;
		};
		QList<Enum> enums;

		struct Property : public BaseParam {
			bool generateReset = false;

			inline Property(const BaseParam &base = {}) :
				BaseParam(base)
			{}
		};
		QList<Property> properties;
	} data;

	void build() override;

	void readData();
	XmlContent::Enum readEnum();
	XmlContent::Property readProperty();

	void generateApiObject();
	void generateApiGadget();

	QString setter(const QString &name);

	void writeEnums();
	void writeFlagOperators();
	void writeProperties();
	void writeReadDeclarations();
	void writeWriteDeclarations();
	void writeResetDeclarations();
	void writeNotifyDeclarations();
	void writeMemberDeclarations();
	void writeEqualsDeclaration();
	void writeSourceIncludes();
	void writeReadDefinitions();
	void writeWriteDefinitions();
	void writeResetDefinitions();
	void writeEqualsDefinition();
	void writePrivateClass();
	void writeDataClass();
	void writeMemberDefinitions(bool skipComma);
	void writeListConverter();
};

#endif // OBJECTBUILDER_H
