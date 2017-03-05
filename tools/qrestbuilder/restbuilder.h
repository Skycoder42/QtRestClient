#ifndef RESTBUILDER_H
#define RESTBUILDER_H

#include <QObject>

class RestBuilder : public QObject
{
	Q_OBJECT
public:
	explicit RestBuilder(QObject *parent = nullptr);

	int build(const QString &in, const QString &hOut, const QString &cppOut);
};

#endif // RESTBUILDER_H
