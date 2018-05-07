#ifndef CLASSBUILDER_H
#define CLASSBUILDER_H

#include "restbuilder.h"
#include <QVersionNumber>
#if __cplusplus >= 201703L
#include <variant>
#endif

class ClassBuilder : public RestBuilder
{
	Q_OBJECT

public:
	explicit ClassBuilder(QXmlStreamReader &reader, QObject *parent = nullptr);

	static bool canReadType(const QString &type);

private:
	struct Expression {
		bool expr = false;
		QString value;
	};

	struct FixedParam : public Expression {
		QString key;
	};

	struct RestAccess {
		QString name;
		QString exportKey;
		QString base;
		QString except;

		QList<Include> includes;

		struct Class {
			QString key;
			QString type;
		};
		QList<Class> classes;

		struct Method {
			QString name;
			QString verb;
			QString body;
			QString returns;
			QString except;
			bool postParams = false; //TODO use in code

#if __cplusplus >= 201703L
			using PathInfoBase = QList<std::variant<Expression, BaseParam>>;
			using PathInfo = std::variant<PathInfoBase, QString>;
#else
			struct {
				bool isParams = false;
				union {
					Expression path;
					BaseParam pathParams;
				};
			} PathInfoBase;
			struct {
				bool isPath = false;
				union {
					QList<PathInfoBase> path;
					QString url;
				};
			} PathInfo;
#endif
			PathInfo path;
			QList<BaseParam> params;
			QList<FixedParam> headers;
		};
		QList<Method> methods;
	} data;

	struct RestClass {
		Expression path;
	} classData;

	struct RestApi {
		QString globalName;
		bool autoCreate = false;

		Expression baseUrl;
		QVersionNumber apiVersion;
		QList<FixedParam> params;
		QList<FixedParam> headers;
	} apiData;

	bool isApi = false;

	void build() override;

	void readData();
	Expression readExpression();
	FixedParam readFixedParam();
	RestAccess::Class readClass();
	RestAccess::Method readMethod();

	void generateClass();
	void generateApi();

	void writeClassBeginDeclaration();
	void writeClassMainDeclaration();
	void writeClassBeginDefinition();
	void writeClassMainDefinition();

	void methodParams(const RestAccess::Method &method);
	QString writeExpression(const Expression &expression, bool asString);

	void generateFactoryDeclaration();
	void writeFactoryDeclarations();
	void writeClassDeclarations();
	void writeMethodDeclarations();
	void writeMemberDeclarations();
	void writePrivateDefinitions();
	void generateFactoryDefinition();
	void writeFactoryDefinitions();
	void writeClassDefinitions();
	void writeMethodDefinitions();
	void writeMemberDefinitions();

	void writeLocalApiGeneration();
	void writeGlobalApiGeneration();
	void writeApiCreation();

	bool writeMethodPath(const ClassBuilder::RestAccess::Method::PathInfo &info);
};

#if __cplusplus < 201703L
#error implement std::get and std::holds_alternative
#endif

#endif // CLASSBUILDER_H
