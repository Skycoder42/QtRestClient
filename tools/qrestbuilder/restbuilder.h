#ifndef RESTBUILDER_H
#define RESTBUILDER_H

#include <tuple>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QTextStream>
#include <qrestbuilder.h>

class RestBuilder
{
public:
	virtual ~RestBuilder();

	void build(const QString &in, const QString &hOut, const QString &cppOut);

protected:
	virtual void build() = 0;

	QString exportedName(const QString &name, const RestBuilderXmlReader::optional<QString> &exportKey) const;
	QString nsName(const QString &name, const RestBuilderXmlReader::optional<QString> &nspace) const;
	QString nsInject(const QString &name, const QString &prefix) const;
	QString boolValue(bool value) const;

	void writeIncludes(const QList<RestBuilderXmlReader::Include> &includes = {});
	QString writeParamDefault(const RestBuilderXmlReader::BaseParam &param);
	QString writeParamArg(const RestBuilderXmlReader::BaseParam &param, bool withDefault);

	QString fileName;
	QTextStream header;
	QTextStream source;

private:
	void writeIncGuardBegin();
	void writeIncGuardEnd();
};

#endif // RESTBUILDER_H
