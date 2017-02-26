#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QTimer>
using namespace QtRestClient;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	client(new RestClient(this))
{
	ui->setupUi(this);

	client->setBaseUrl(QStringLiteral("http://api.example.com"));

	connect(client->manager(), &QNetworkAccessManager::authenticationRequired,
			this, &MainWindow::authenticate);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_addParamButton_clicked()
{
	auto item = new QTreeWidgetItem(ui->paramTreeWidget);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	ui->paramTreeWidget->setCurrentItem(item);
	ui->paramTreeWidget->editItem(item);
}

void MainWindow::on_removeParamButton_clicked()
{
	auto current = ui->paramTreeWidget->currentItem();
	if(current)
		delete current;
}

void MainWindow::on_addHeaderButton_clicked()
{
	auto item = new QTreeWidgetItem(ui->headersTreeWidget);
	item->setFlags(item->flags() | Qt::ItemIsEditable);
	ui->headersTreeWidget->setCurrentItem(item);
	ui->headersTreeWidget->editItem(item);
}

void MainWindow::on_removeHeaderButton_clicked()
{
	auto current = ui->headersTreeWidget->currentItem();
	if(current)
		delete current;
}

void MainWindow::on_pushButton_clicked()
{
	ui->tabWidget->setCurrentWidget(ui->replyTab);

	QVariantHash params;
	for(auto i = 0; i < ui->paramTreeWidget->topLevelItemCount(); i++) {
		auto item = ui->paramTreeWidget->topLevelItem(i);
		params.insert(item->text(0), item->text(1));
	}

	HeaderHash headers;
	for(auto i = 0; i < ui->headersTreeWidget->topLevelItemCount(); i++) {
		auto item = ui->headersTreeWidget->topLevelItem(i);
		headers.insert(item->text(0).toUtf8(), item->text(1).toUtf8());
	}

	QJsonParseError error;
	QJsonDocument body;
	if(!ui->bodyJsonEdit->toPlainText().isEmpty())
		body = QJsonDocument::fromJson(ui->bodyJsonEdit->toPlainText().toUtf8(), &error);
	else
		error.error = QJsonParseError::NoError;
	RestReply *reply;
	if(error.error != QJsonParseError::NoError) {
		ui->codeLineEdit->setText(QString::number(error.error));
		ui->networkErrorLineEdit->setText(error.errorString());
		ui->replyJsonEdit->clear();
		return;
	} else if(body.isObject()) {
		reply = client->rootClass()->callJson(ui->verbComboBox->currentText().toUtf8(),
											  QUrl(ui->urlLineEdit->text()),
											  body.object(),
											  params,
											  headers);
	} else if(body.isArray()) {
		reply = client->rootClass()->callJson(ui->verbComboBox->currentText().toUtf8(),
											  QUrl(ui->urlLineEdit->text()),
											  body.array(),
											  params,
											  headers);
	} else {
		reply = client->rootClass()->callJson(ui->verbComboBox->currentText().toUtf8(),
											  QUrl(ui->urlLineEdit->text()),
											  params,
											  headers);
	}

	reply->enableAutoDelete();
	connect(reply, &RestReply::succeeded, this, [=](int status, QJsonValue value){
		ui->codeLineEdit->setText(QString::number(status));
		ui->networkErrorLineEdit->clear();
		QJsonDocument doc;
		if(value.isObject())
			doc = QJsonDocument(value.toObject());
		else
			doc = QJsonDocument(value.toArray());
		ui->replyJsonEdit->setPlainText(QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
		QTimer::singleShot(2000, this, &MainWindow::zeroBars);
	});
	connect(reply, &RestReply::failed, this, [=](int status, QJsonValue value){
		ui->codeLineEdit->setText(QString::number(status));
		ui->networkErrorLineEdit->setText("Request Failed! See JSON for more details!");
		QJsonDocument doc;
		if(value.isObject())
			doc = QJsonDocument(value.toObject());
		else
			doc = QJsonDocument(value.toArray());
		ui->replyJsonEdit->setPlainText(QString::fromUtf8(doc.toJson(QJsonDocument::Indented)));
		QTimer::singleShot(2000, this, &MainWindow::zeroBars);
	});
	connect(reply, &RestReply::error, this, [=](QString errorString, int type){
		ui->codeLineEdit->setText(QString::number(type));
		ui->networkErrorLineEdit->setText(errorString);
		ui->replyJsonEdit->clear();
		QTimer::singleShot(2000, this, &MainWindow::zeroBars);
	});

	ui->requestUrlLineEdit->setText(reply->networkReply()->url().toString());
	connect(reply, &RestReply::uploadProgress, this, [this](int c, int m){
		ui->uploadBar->setMaximum(m);
		ui->uploadBar->setValue(c);
	});
	connect(reply, &RestReply::downloadProgress, this, [this](int c, int m){
		ui->downloadBar->setMaximum(m);
		ui->downloadBar->setValue(c);
	});
}

void MainWindow::authenticate(QNetworkReply *, QAuthenticator *auth)
{
	auth->setUser(ui->usernameLineEdit->text());
	auth->setPassword(ui->passwordLineEdit->text());
}

void MainWindow::zeroBars()
{
	ui->uploadBar->setMaximum(100);
	ui->uploadBar->setValue(0);
	ui->downloadBar->setMaximum(100);
	ui->downloadBar->setValue(0);
}
