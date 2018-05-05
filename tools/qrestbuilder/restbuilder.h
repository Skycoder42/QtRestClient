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
	virtual void build() = 0;

	template <typename T = QString>
	T readAttrib(const QString &key, const T &defaultValue = {}) const;

	Q_NORETURN void throwFile(const QFileDevice &file) const;
	Q_NORETURN void throwReader(const QString &overwriteError = {}) const;
	Q_NORETURN static void throwReader(QXmlStreamReader &stream, const QString &overwriteError = {});
	Q_NORETURN void throwChild();
	void checkError();

	void transformIncludes(const QStringList &extras = {});

	QString fileName;
	QString className;
	QString exportedClassName;
	QXmlStreamReader &reader;
	QTextStream header;
	QTextStream source;

private:
	void writeIncGuardBegin();
	void writeIncGuardEnd();
};

template<typename T>
T RestBuilder::readAttrib(const QString &key, const T &defaultValue) const
{
	if(reader.attributes().hasAttribute(key))
		return QVariant(reader.attributes().value(key).toString()).template value<T>();
	else
		return defaultValue;
}

template<>
bool RestBuilder::readAttrib<bool>(const QString &key, const bool &defaultValue) const;

#endif // RESTBUILDER_H
