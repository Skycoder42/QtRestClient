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

private:
	Ui::MainWindow *ui;
	ExampleApi *api;

	void onError(QString error, int code, QtRestClient::RestReply::ErrorType type);
};

#endif // MAINWINDOW_H
