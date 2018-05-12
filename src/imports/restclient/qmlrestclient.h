#ifndef QTRESTCLIENT_QMLRESTCLIENT_H
#define QTRESTCLIENT_QMLRESTCLIENT_H

#include <QtCore/QObject>
#include <QtRestClient/RestClient>

#include "qmlrestclass.h"

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtRestClient {

/*! @brief The QML version of ::QtRestClient::RestClient
 *
 * @since 2.0
 */
class RestClient : public ::QtRestClient::RestClient
{
	Q_OBJECT

	/*! @brief A helper property to declare RestClasses for the client
	 *
	 * @default{<i>empty</i>}
	 * @accessors{
	 *	@memberAc{classes}
	 * }
	 * @sa de::skycoder42::QtRestClient::RestClass
	 */
	Q_PROPERTY(QQmlListProperty<de::skycoder42::QtRestClient::RestClass> classes READ classes)
#else
namespace QtRestClient {

class QmlRestClient : public RestClient
{
	Q_OBJECT

	Q_PROPERTY(QQmlListProperty<QtRestClient::QmlRestClass> classes READ classes)
#endif

	Q_CLASSINFO("DefaultProperty", "classes")

public:
	//! Default constructor with object parent
	explicit QmlRestClient(QObject *parent = nullptr);

	//! @private
	QQmlListProperty<QmlRestClass> classes();

private:
	QList<QmlRestClass*> _childClasses;
};

}

#endif // QTRESTCLIENT_QMLRESTCLIENT_H
