#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <api.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_reloadButton_clicked();
	void on_addButton_clicked();
	void on_updateButton_clicked();
	void on_deleteButton_clicked();

private:
	Ui::MainWindow *ui;
	ExampleApi *api;

	Post getPost() const;
	void setPost(const Post &post);
	void clearPost();
	void onError(bool isLoad, QString error, int code, QtRestClient::RestReply::ErrorType type);
};

#endif // MAINWINDOW_H
