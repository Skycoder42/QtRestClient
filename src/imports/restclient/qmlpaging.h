#ifndef QTRESTCLIENT_QMLPAGING_H
#define QTRESTCLIENT_QMLPAGING_H

#include <QtCore/QObject>
#include <QtQml/QJSValue>
#include <QtRestClient/IPaging>
#include <QtRestClient/RestReply>
#include <QtRestClient/RestClient>
#include <QJSEngine>

#ifdef DOXYGEN_RUN
namespace de::skycoder42::QtRestClient {

/*! @brief The QML version of ::QtRestClient::Paging
 *
 * @since 2.0
 */
class Paging
#else
namespace QtRestClient {

class QmlPaging
#endif
{
	Q_GADGET

	/*! @copybrief ::QtRestClient::IPaging::items
	 *
	 * @default{<i>empty</i>}
	 * @accessors{
	 *	@memberAc{items}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::IPaging::items
	 */
	Q_PROPERTY(QVariantList items READ items CONSTANT)
	/*! @copybrief ::QtRestClient::IPaging::total
	 *
	 * @default{`INT_MAX`}
	 * @accessors{
	 *	@memberAc{total}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::IPaging::total
	 */
	Q_PROPERTY(int total READ total CONSTANT)
	/*! @copybrief ::QtRestClient::IPaging::offset
	 *
	 * @default{`-1`}
	 * @accessors{
	 *	@memberAc{offset}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::IPaging::offset
	 */
	Q_PROPERTY(int offset READ offset CONSTANT)
	/*! @copybrief ::QtRestClient::IPaging::hasNext
	 *
	 * @default{`false`}
	 * @accessors{
	 *	@memberAc{hasNext}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::IPaging::hasNext
	 */
	Q_PROPERTY(bool hasNext READ hasNext CONSTANT)
	/*! @copybrief ::QtRestClient::IPaging::next
	 *
	 * @default{<i>invalid</i>}
	 * @accessors{
	 *	@memberAc{nextUrl}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::IPaging::next
	 */
	Q_PROPERTY(QUrl nextUrl READ nextUrl CONSTANT)
	/*! @copybrief ::QtRestClient::IPaging::hasPrevious
	 *
	 * @default{`false`}
	 * @accessors{
	 *	@memberAc{hasPrevious}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::IPaging::hasPrevious
	 */
	Q_PROPERTY(bool hasPrevious READ hasPrevious CONSTANT)
	/*! @copybrief ::QtRestClient::IPaging::previous
	 *
	 * @default{<i>invalid</i>}
	 * @accessors{
	 *	@memberAc{previousUrl}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::IPaging::previous
	 */
	Q_PROPERTY(QUrl previousUrl READ previousUrl CONSTANT)
	/*! @copybrief ::QtRestClient::IPaging::properties
	 *
	 * @default{<i>empty</i>}
	 * @accessors{
	 *	@memberAc{properties}
	 *  @readonlyAc
	 * }
	 * @sa ::QtRestClient::IPaging::properties
	 */
	Q_PROPERTY(QVariantMap properties READ properties CONSTANT)

public:
	//! @private
	explicit QmlPaging() = default;
	//! @private
	explicit QmlPaging(IPaging *iPaging, RestClient *client, QJSEngine *engine);

	//! @private
	static QmlPaging create(RestClient *client, QJSEngine *engine, const QJsonObject &obj);

	//! Performs a request for the next paging object
	Q_INVOKABLE QtRestClient::RestReply *next();
	//! Performs a request for the previous paging object
	Q_INVOKABLE QtRestClient::RestReply *previous();

	//! @private
	QVariantList items() const;
	//! @private
	int total() const;
	//! @private
	int offset() const;
	//! @private
	bool hasNext() const;
	//! @private
	QUrl nextUrl() const;
	//! @private
	bool hasPrevious() const;
	//! @private
	QUrl previousUrl() const;
	//! @private
	QVariantMap properties() const;

public Q_SLOTS:
	/*! Iterates over all paging objects
	 *
	 * @param iterator The iterator to be be called for every element iterated over
	 * @param to The upper limit of how far the iteration should go (-1 means no limit)
	 * @param from The lower limit from where the iteration should start
	 *
	 * @sa ::QtRestClient::Paging::iterate
	 */
	void iterate(const QJSValue &iterator,
				 int to = -1, int from = 0);
	/*! Iterates over all paging objects
	 *
	 * @param iterator The iterator to be be called for every element iterated over
	 * @param failureHandler Will be passed to GenericRestReply::onFailed for all replies
	 * @param errorHandler Will be passed to GenericRestReply::onError for all replies
	 * @param to The upper limit of how far the iteration should go (-1 means no limit)
	 * @param from The lower limit from where the iteration should start
	 *
	 * @sa ::QtRestClient::Paging::iterate
	 */
	void iterate(const QJSValue &iterator,
				 const QJSValue &failureHandler,
				 const QJSValue &errorHandler,
				 int to = -1, int from = 0);

private:
	QJSEngine *_engine = nullptr;
	RestClient *_client = nullptr;
	QSharedPointer<IPaging> _paging;

	int internalIterate(QJSValue iterator, int from, int to) const;
};

}

Q_DECLARE_METATYPE(QtRestClient::QmlPaging)

#endif // QTRESTCLIENT_QMLPAGING_H
