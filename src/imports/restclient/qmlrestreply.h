#ifndef QTRESTCLIENT_QMLRESTREPLY_H
#define QTRESTCLIENT_QMLRESTREPLY_H

#include <QtCore/QPointer>

#include <QtRestClient/RestReply>

#include <QtQml/QJSValue>
#include <QtQml/QJSEngine>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtRestClient {

/*! @brief The QML version of ::QtRestClient::RestReply
 *
 * @extends QtQml.QtObject
 * @since 3.0
 *
 * This is a special version that is returned by the RestClass to be able to set handlers.
 * The original reply can be accessed via the RestReply::reply property.
 */
class RestReply
#else
namespace QtRestClient {

class QmlRestReply : public QObject
#endif
{
	Q_OBJECT

	/*! @brief The original ::QtRestClient::RestReply that this one wrapps
	 *
	 * @default{<i>auto</i>}
	 * @accessors{
	 *	@memberAc{reply}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::RestReply
	 */
	Q_PROPERTY(QtRestClient::RestReply *reply MEMBER _reply CONSTANT)

public:
	//! @private
	explicit QmlRestReply(RestReply *reply,
						  QJSEngine *engine,
						  QObject *parent = nullptr);

public Q_SLOTS:
	/*! @brief Add a method to be called when the request has been completed
	 *
	 * @param completedHandler a JS callback to be called with the reply
	 *
	 * The handlers arguments are:
	 * - The HTTP-Status code (int)
	 */
	void addCompletedHandler(const QJSValue &completedHandler);
	/*! @brief Add a method to be called when the request has been completed successfully
	 *
	 * @param succeededHandler a JS callback to be called with the reply
	 *
	 * The handlers arguments are:
	 * - The HTTP-Status code (int)
	 * - The deserialized Content of the reply (deserialized to a JS object)
	 */
	void addSucceededHandler(const QJSValue &succeededHandler);
	/*! @brief Add a method to be called if the request has failed
	 *
	 * @param failedHandler a JS callback to be called with the reply
	 *
	 * The handlers arguments are:
	 * - The HTTP-Status code (int)
	 * - The deserialized Content of the reply (deserialized to a JS object)
	 */
	void addFailedHandler(const QJSValue &failedHandler);
	/*! @brief Add a method to be called if an error occured
	 *
	 * @param errorHandler a JS callback to be called with the reply
	 *
	 * The handlers arguments are:
	 * - The error string (string)
	 * - The error code (int)
	 * - The error type (::QtRestClient::RestReply::Error)
	 */
	void addErrorHandler(const QJSValue &errorHandler);

private:
	QPointer<RestReply> _reply;
	QJSEngine *_engine;

	bool checkOk(const QJSValue &fn) const;
};

}

#endif // QMLRESTREPLY_H
