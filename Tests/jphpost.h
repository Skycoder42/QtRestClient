#ifndef JPHPOST_H
#define JPHPOST_H

#include <QtRestClient>

class JphPost : public QtRestClient::RestObject
{
	Q_OBJECT

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(int userId MEMBER userId)
	Q_PROPERTY(QString title MEMBER title)
	Q_PROPERTY(QString body MEMBER body)

public:
	Q_INVOKABLE JphPost(QObject *parent = nullptr);
	JphPost(int id, int userId, QString title, QString body, QObject *parent = nullptr);

	static JphPost *createDefault(QObject *parent);

	int userId;
	int id;
	QString title;
	QString body;
};

#endif // JPHPOST_H
