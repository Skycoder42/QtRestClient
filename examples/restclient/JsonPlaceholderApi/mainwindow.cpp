#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::MainWindow),
	api(new ExampleApi(this))
{
	ui->setupUi(this);
	api->restClient()->setModernAttributes();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::on_reloadButton_clicked()
{
	ui->postsTreeWidget->clear();
	ui->reloadButton->setEnabled(false);
	api->posts()->listPosts()->onSucceeded([this](int code, QList<Post> posts){
		qDebug() << "reply code:" << code;
		ui->reloadButton->setEnabled(true);
		foreach(auto post, posts) {
			new QTreeWidgetItem(ui->postsTreeWidget, {
									QString::number(post.id()),
									QString::number(post.userId()),
									post.title(),
									post.body()
								});
		}
	})->onAllErrors([this](QString error, int code, QtRestClient::RestReply::ErrorType type){
		onError(error, code, type);
	});
}

void MainWindow::onError(QString error, int code, QtRestClient::RestReply::ErrorType type)
{
	QMessageBox::critical(this,
						  QStringLiteral("Error of type %1").arg(type),
						  QStringLiteral("Code: %1\nError Text: %2").arg(code).arg(error));
}
