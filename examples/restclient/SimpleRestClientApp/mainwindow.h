#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtNetwork/QAuthenticator>
#include <QtWidgets/QMainWindow>
#include <QtRestClient>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_addParamButton_clicked();
	void on_removeParamButton_clicked();
	void on_addHeaderButton_clicked();
	void on_removeHeaderButton_clicked();
	void on_pushButton_clicked();

	void authenticate(QNetworkReply *reply, QAuthenticator *auth);
	void zeroBars();

private:
	Ui::MainWindow *ui;
	QtRestClient::RestClient *client;
};

#endif // MAINWINDOW_H
