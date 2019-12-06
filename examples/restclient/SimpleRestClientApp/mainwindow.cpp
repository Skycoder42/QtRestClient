#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>
#include <QtRestClient/RestClass>
using namespace QtRestClient;

Q_DECLARE_OPERATORS_FOR_FLAGS(QCborValue::DiagnosticNotationOptions)

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow{parent},
	_ui{new Ui::MainWindow{}},
	_client{new RestClient{this}}
{
	_ui->setupUi(this);

	_client->setBaseUrl(QStringLiteral("http://api.example.com"));

	connect(_client->manager(), &QNetworkAccessManager::authenticationRequired,
			this, &MainWindow::authenticate);
}

MainWindow::~MainWindow() = default;

void MainWindow::on_cborButton_toggled(bool checked)
{
	if (checked)
		_client->setDataMode(RestClient::DataMode::Cbor);
}

void MainWindow::on_jsonButton_toggled(bool checked)
{
	if (checked)
		_client->setDataMode(RestClient::DataMode::Json);
}

void MainWindow::on_addParamButton_clicked()
{
	auto item = new QTreeWidgetItem(_ui->paramTreeWidget);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	_ui->paramTreeWidget->setCurrentItem(item);
	_ui->paramTreeWidget->editItem(item);
}

void MainWindow::on_removeParamButton_clicked()
{
	auto current = _ui->paramTreeWidget->currentItem();
	if (current)
		delete current;
}

void MainWindow::on_addHeaderButton_clicked()
{
	auto item = new QTreeWidgetItem(_ui->headersTreeWidget);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	_ui->headersTreeWidget->setCurrentItem(item);
	_ui->headersTreeWidget->editItem(item);
}

void MainWindow::on_removeHeaderButton_clicked()
{
	auto current = _ui->headersTreeWidget->currentItem();
	if (current)
		delete current;
}

void MainWindow::on_pushButton_clicked()
{
	_ui->tabWidget->setCurrentWidget(_ui->replyTab);

	try {
		QVariantHash params;
		for (auto i = 0; i < _ui->paramTreeWidget->topLevelItemCount(); i++) {
			auto item = _ui->paramTreeWidget->topLevelItem(i);
			params.insert(item->text(0), item->text(1));
		}

		HeaderHash headers;
		for (auto i = 0; i < _ui->headersTreeWidget->topLevelItemCount(); i++) {
			auto item = _ui->headersTreeWidget->topLevelItem(i);
			headers.insert(item->text(0).toUtf8(), item->text(1).toUtf8());
		}

		auto body = parseBody();
		RestReply *reply;
		if (std::holds_alternative<QCborValue>(body)) {
			reply = _client->rootClass()->callRaw(_ui->verbComboBox->currentText().toUtf8(),
												  QUrl{_ui->urlLineEdit->text()},
												  std::get<QCborValue>(body),
												  params,
												  headers);
		} else if(std::holds_alternative<QJsonValue>(body)) {
			reply = _client->rootClass()->callRaw(_ui->verbComboBox->currentText().toUtf8(),
												  QUrl{_ui->urlLineEdit->text()},
												  std::get<QJsonValue>(body),
												  params,
												  headers);
		} else {
			reply = _client->rootClass()->callRaw(_ui->verbComboBox->currentText().toUtf8(),
												  QUrl{_ui->urlLineEdit->text()},
												  params,
												  headers);
		}

		reply->onSucceeded(this, [=](int status, const RestReply::DataType &value){
			_ui->codeLineEdit->setText(QString::number(status));
			_ui->networkErrorLineEdit->clear();
			writeBody(value);
			QTimer::singleShot(2000, this, &MainWindow::zeroBars);
		});
		reply->onFailed(this, [=](int status, const RestReply::DataType &value){
			_ui->codeLineEdit->setText(QString::number(status));
			_ui->networkErrorLabel->setText(tr("Request failure:"));
			_ui->networkErrorLineEdit->setText(tr("Request Failed! See JSON for more details!"));
			writeBody(value);
			QTimer::singleShot(2000, this, &MainWindow::zeroBars);
		});
		reply->onError(this, [=](const QString &errorString, int code, RestReply::Error type){
			_ui->codeLineEdit->setText(QString::number(code));
			switch (type) {
			case RestReply::Error::Network:
				_ui->networkErrorLabel->setText(tr("Network error:"));
				break;
			case RestReply::Error::Parser:
				_ui->networkErrorLabel->setText(tr("JSON/CBOR parse error:"));
				break;
			case RestReply::Error::Failure:
				_ui->networkErrorLabel->setText(tr("Request failure:"));
				break;
			case RestReply::Error::Deserialization:
				_ui->networkErrorLabel->setText(tr("Deserialization error:"));
				break;
			}
			_ui->networkErrorLineEdit->setText(errorString);
			_ui->replyJsonEdit->clear();
			QTimer::singleShot(2000, this, &MainWindow::zeroBars);
		});

		_ui->requestUrlLineEdit->setText(reply->networkReply()->url().toString());
		connect(reply, &RestReply::uploadProgress, this, [this](int c, int m){
			_ui->uploadBar->setMaximum(m);
			_ui->uploadBar->setValue(c);
		});
		connect(reply, &RestReply::downloadProgress, this, [this](int c, int m){
			_ui->downloadBar->setMaximum(m);
			_ui->downloadBar->setValue(c);
		});
	} catch (QJsonParseError &e) {
		_ui->codeLineEdit->setText(QString::number(e.error));
		_ui->networkErrorLineEdit->setText(e.errorString());
		_ui->replyJsonEdit->clear();
	} catch (std::exception &e) {
		_ui->codeLineEdit->clear();
		_ui->networkErrorLineEdit->setText(QString::fromUtf8(e.what()));
		_ui->replyJsonEdit->clear();
	}
}

void MainWindow::authenticate(QNetworkReply *, QAuthenticator *auth)
{
	auth->setUser(_ui->usernameLineEdit->text());
	auth->setPassword(_ui->passwordLineEdit->text());
}

void MainWindow::zeroBars()
{
	_ui->uploadBar->setMaximum(100);
	_ui->uploadBar->setValue(0);
	_ui->downloadBar->setMaximum(100);
	_ui->downloadBar->setValue(0);
}

RestReply::DataType MainWindow::parseBody() const
{
	const auto data = _ui->bodyJsonEdit->toPlainText();
	if (data.isEmpty())
		return std::nullopt;

	QJsonParseError error;
	auto doc = QJsonDocument::fromJson(data.toUtf8(), &error);
	QJsonValue body = QJsonValue::Null;
	if (error.error != QJsonParseError::NoError)
		throw error;
	else if (doc.isObject())
		body = doc.object();
	else if (doc.isArray())
		body = doc.array();

	if (_client->dataMode() == RestClient::DataMode::Cbor)
		return QCborValue::fromJsonValue(body);
	else
		return body;
}

void MainWindow::writeBody(const RestReply::DataType &data)
{
	if (std::holds_alternative<QCborValue>(data))
		_ui->replyJsonEdit->setPlainText(std::get<QCborValue>(data).toDiagnosticNotation(QCborValue::LineWrapped | QCborValue::ExtendedFormat));
	else if (std::holds_alternative<QJsonValue>(data)) {
		const auto jValue = std::get<QJsonValue>(data);
		QJsonDocument doc;
		if (jValue.isObject())
			doc = QJsonDocument{jValue.toObject()};
		else
			doc = QJsonDocument{jValue.toArray()};
		_ui->replyJsonEdit->setPlainText(QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
	} else
		_ui->replyJsonEdit->clear();
}
