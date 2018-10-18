#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
using namespace QtRestClient;

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
		ui->loadStatusLabel->setText(QStringLiteral("Status: %1").arg(code));
		ui->reloadButton->setEnabled(true);
		for(const auto& post : qAsConst(posts)) {
			new QTreeWidgetItem(ui->postsTreeWidget, {
									QString::number(post.id()),
									QString::number(post.userId()),
									post.title(),
									post.body()
								});
		}
	})->onAllErrors([this](QString error, int code, RestReply::ErrorType type){
		onError(true, error, code, type);
	});
}

void MainWindow::onError(bool isLoad, const QString& error, int code, RestReply::ErrorType type)
{
	if(isLoad)
		ui->loadStatusLabel->setText(QStringLiteral("Status: %1").arg(type == RestReply::FailureError ? code : -1));
	else
		ui->editStatusLabel->setText(QString::number(type == RestReply::FailureError ? code : -1));

	QMessageBox::critical(this,
						  QStringLiteral("Error of type %1").arg(type),
						  QStringLiteral("Code: %1\nError Text: %2").arg(code).arg(error));
}

void MainWindow::on_addButton_clicked()
{
	api->posts()->savePost(getPost())->onSucceeded([this](int code, Post post){
		ui->editStatusLabel->setText(QString::number(code));
		setPost(post);
	})->onAllErrors([this](QString error, int code, RestReply::ErrorType type){
		onError(false, error, code, type);
	});
}

void MainWindow::on_updateButton_clicked()
{
	api->posts()->updatePost(getPost(), ui->idSpinBox->value())->onSucceeded([this](int code, Post post){
		ui->editStatusLabel->setText(QString::number(code));
		setPost(post);
	})->onAllErrors([this](QString error, int code, RestReply::ErrorType type){
		onError(false, error, code, type);
	});
}

void MainWindow::on_deleteButton_clicked()
{
	api->posts()->deletePost(ui->idSpinBox->value())->onSucceeded([this](int code){
		ui->editStatusLabel->setText(QString::number(code));
		clearPost();
	})->onAllErrors([this](QString error, int code, RestReply::ErrorType type){
		onError(false, error, code, type);
	});
}

Post MainWindow::getPost() const
{
	Post p;
	p.setId(ui->idSpinBox->value());
	p.setUserId(ui->userIDSpinBox->value());
	p.setTitle(ui->titleLineEdit->text());
	p.setBody(ui->bodyLineEdit->text());
	return p;
}

void MainWindow::setPost(const Post &post)
{
	ui->replyIdBox->setValue(post.id());
	ui->replyUserBox->setValue(post.userId());
	ui->replyTitleBox->setText(post.title());
	ui->replyBodyBox->setText(post.body());
}

void MainWindow::clearPost()
{
	ui->replyIdBox->clear();
	ui->replyUserBox->clear();
	ui->replyTitleBox->clear();
	ui->replyBodyBox->clear();
}
