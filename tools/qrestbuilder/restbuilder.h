#ifndef RESTBUILDER_H
#define RESTBUILDER_H

#include <tuple>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QXmlStreamReader>
#include <QTextStream>

class RestBuilder : public QObject
{
	Q_OBJECT

public:
	explicit RestBuilder(QXmlStreamReader &inStream, QObject *parent = nullptr);

	static QString readType(QXmlStreamReader &inStream);

	void build(const QString &in, const QString &hOut, const QString &cppOut);

protected:
	struct Include {
		bool local;
		QString include;
	};

	struct BaseParam {
		QString key;
		QString type;
		bool asStr = false;
		QString defaultValue;
	};

	virtual void build() = 0;

	template <typename T = QString>
	T readAttrib(const QString &key, const T &defaultValue = {}, bool required = false) const;
	Include readInclude();
	BaseParam readBaseParam();

	Q_NORETURN void throwFile(const QFileDevice &file) const;
	Q_NORETURN void throwReader(const QString &overwriteError = {}) const;
	Q_NORETURN static void throwReader(QXmlStreamReader &stream, const QString &overwriteError = {});
	Q_NORETURN void throwChild();
	void checkError();

	QString exportedName(const QString &name, const QString &exportKey) const;
	QString nsName(const QString &name, const QString &nspace) const;
	QString nsInject(const QString &name, const QString &prefix) const;

	void writeIncludes(const QList<Include> &includes = {});
	QString writeParamDefault(const BaseParam &param);
	QString writeParamArg(const BaseParam &param, bool withDefault);

	QString fileName;
	QXmlStreamReader &reader;
	QTextStream header;
	QTextStream source;

private:
	void writeIncGuardBegin();
	void writeIncGuardEnd();
};

template<typename T>
T RestBuilder::readAttrib(const QString &key, const T &defaultValue, bool required) const
{
	if(reader.attributes().hasAttribute(key))
		return QVariant(reader.attributes().value(key).toString()).template value<T>();
	else if(required)
		throwReader(tr("Required attribute \"%1\" but was not set").arg(key));
	else
		return defaultValue;
}

template<>
bool RestBuilder::readAttrib<bool>(const QString &key, const bool &defaultValue, bool required) const;

#endif // RESTBUILDER_H
