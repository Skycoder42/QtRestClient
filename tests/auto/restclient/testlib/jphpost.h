#ifndef JPHPOST_H
#define JPHPOST_H

#include <QtRestClient>

class JphPost : public QObject
{
	Q_OBJECT

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(int userId MEMBER userId)
	Q_PROPERTY(QString title MEMBER title)
	Q_PROPERTY(QString body MEMBER body)

public:
	Q_INVOKABLE JphPost(QObject *parent = nullptr);
	JphPost(int id, int userId, QString title, QString body, QObject *parent = nullptr);

	static bool equals(const JphPost *left, const QObject *right);
	virtual bool equals(const QObject *other) const;

	template<typename T>
	static bool listEquals(const QList<T*> &left, const QList<T*> &right);

	static JphPost *createDefault(QObject *parent);
	static JphPost *createFirst(QObject *parent);

	int id = 0;
	int userId = 0;
	QString title;
	QString body;
};

class JphPostSimple : public QtRestClient::Simple<JphPost*>
{
	Q_OBJECT

	Q_PROPERTY(int id MEMBER id)
	Q_PROPERTY(QString title MEMBER title)
	Q_PROPERTY(QUrl href MEMBER href)

public:
	Q_INVOKABLE JphPostSimple(QObject *parent = nullptr);
	JphPostSimple(int id, QString title, QUrl href, QObject *parent = nullptr);

	QTRESTCLIENT_EXT_HREF_PROP(href)

	int id = 0;
	QString title;
	QUrl href;
};

template<typename T>
bool JphPost::listEquals(const QList<T*> &left, const QList<T*> &right)
{
	static_assert(std::is_base_of<QObject, T>::value, "T must inherit QObject!");
	if (left.size() != right.size())
		return false;
	else {
		for (auto i = 0; i < left.size(); i++) {
			if (!equals(qobject_cast<JphPost*>(left[i]), qobject_cast<JphPost*>(right[i])))
				return false;
		}
		return true;
	}
}


#endif // JPHPOST_H
