#include "testlib.h"

#include <jphpost.h>
using namespace QtRestClient;
using namespace QtJsonSerializer;

class PagingModelTest : public QObject
{
	Q_OBJECT

private Q_SLOTS:
	void initTestCase();
	void cleanupTestCase();

	void testInitModel();
	void testReplyInitModel();
	void testJsonInitModel();

private:
	HttpServer *server;
	RestClient *client;
	PagingModel *model;

	QAbstractItemModelTester *modelTester;
};

void PagingModelTest::initTestCase()
{
	JsonSerializer::registerListConverters<JphPost*>();
	server = new HttpServer(this);
	QVERIFY(server->setupRoutes());
	server->setAdvancedData();
	client = Testlib::createClient(this);
	QVERIFY(client->serializer());
	client->setBaseUrl(server->url());
	model = new PagingModel{this};

	// create tester
	modelTester = new QAbstractItemModelTester{
		model,
		QAbstractItemModelTester::FailureReportingMode::QtTest,
		this
	};
}

void PagingModelTest::cleanupTestCase()
{
	model->deleteLater();
	model = nullptr;
	client->deleteLater();
	client = nullptr;
	server->deleteLater();
	server = nullptr;
}

void PagingModelTest::testInitModel()
{
	// create mappings
	const auto c1 = model->addColumn(QStringLiteral("title"), "title");
	model->addRole(c1, Qt::ToolTipRole, "id");
	const auto c2 = model->addColumn(QStringLiteral("body"), "body");
	model->addRole(c2, Qt::ToolTipRole, "userId");

	QUrl url {QStringLiteral("pages/0")};
	QVERIFY(url.isValid());
	model->initialize<JphPost*>(url, client->rootClass());

	// let the model populate itself
	QTRY_COMPARE(model->rowCount(), 100);
	QVERIFY(!model->canFetchMore({}));
	QCOMPARE(model->headerData(0).toString(), QStringLiteral("title"));
	QCOMPARE(model->headerData(1).toString(), QStringLiteral("body"));

	// test contained data
	QCOMPARE(model->columnCount(), 2);
	for (auto i = 0; i < 100; ++i) {
		const auto mIndex = model->index(i, 0);
		QVERIFY(mIndex.isValid());
		// test returned object
		const auto post = model->object<JphPost*>(mIndex);
		QVERIFY(post);
		QCOMPARE(post->id, i);
		QCOMPARE(post->userId, qCeil(i/2.0));
		QCOMPARE(post->title, QStringLiteral("Title%1").arg(i));
		QCOMPARE(post->body, QStringLiteral("Body%1").arg(i));
		// test direct model data
		QCOMPARE(model->data(mIndex, model->roleNames().key("id")), QVariant{post->id});
		QCOMPARE(model->data(mIndex, model->roleNames().key("userId")), QVariant{post->userId});
		QCOMPARE(model->data(mIndex, model->roleNames().key("title")), QVariant{post->title});
		QCOMPARE(model->data(mIndex, model->roleNames().key("body")), QVariant{post->body});
		// test mapped model data
		QCOMPARE(model->data(mIndex, Qt::ToolTipRole), QVariant{post->id});
		QCOMPARE(model->data(mIndex.sibling(i, 1), Qt::ToolTipRole), QVariant{post->userId});
		QCOMPARE(model->data(mIndex, Qt::DisplayRole), QVariant{post->title});
		QCOMPARE(model->data(mIndex.sibling(i, 1), Qt::DisplayRole), QVariant{post->body});
	}
}

void PagingModelTest::testReplyInitModel()
{
	QUrl url {QStringLiteral("pages/0")};
	QVERIFY(url.isValid());
	model->initialize(client->rootClass()->get<JphPost*>(url), client->rootClass());

	// let the model populate itself
	QTRY_COMPARE(model->rowCount(), 100);
	QVERIFY(!model->canFetchMore({}));
}

void PagingModelTest::testJsonInitModel()
{
	model->clearColumns();
	QUrl url {QStringLiteral("pages/0")};
	QVERIFY(url.isValid());
	model->initialize(url, client->rootClass());

	// let the model populate itself
	QTRY_COMPARE(model->rowCount(), 100);
	QVERIFY(!model->canFetchMore({}));

	// test contained data
	QCOMPARE(model->columnCount(), 1);
	for (auto i = 0; i < 100; ++i) {
		const auto mIndex = model->index(i, 0);
		QVERIFY(mIndex.isValid());
		// test returned object
		const auto post = model->object<QJsonValue>(mIndex).toObject();
		QCOMPARE(post[QStringLiteral("id")].toInt(), i);
		QCOMPARE(post[QStringLiteral("userId")].toInt(), qCeil(i/2.0));
		QCOMPARE(post[QStringLiteral("title")].toString(), QStringLiteral("Title%1").arg(i));
		QCOMPARE(post[QStringLiteral("body")].toString(), QStringLiteral("Body%1").arg(i));
		// test direct model data
		QCOMPARE(model->data(mIndex, PagingModel::ModelDataRole).toJsonObject(), post);
	}
}

QTEST_MAIN(PagingModelTest)

#include "tst_pagingmodel.moc"
