#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QScopedPointer>
#include <QtNetwork/QAuthenticator>
#include <QtWidgets/QMainWindow>
#include <QtRestClient/RestClient>
#include <QtRestClient/RestReply>

namespace Ui {
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override;

private slots:
	void on_cborButton_toggled(bool checked);
	void on_jsonButton_toggled(bool checked);
	void on_addParamButton_clicked();
	void on_removeParamButton_clicked();
	void on_addHeaderButton_clicked();
	void on_removeHeaderButton_clicked();
	void on_pushButton_clicked();

	void authenticate(QNetworkReply *reply, QAuthenticator *auth);
	void zeroBars();

private:
	QScopedPointer<Ui::MainWindow> _ui;
	QtRestClient::RestClient *_client;

	QtRestClient::RestReply::DataType parseBody() const;
	void writeBody(const QtRestClient::RestReply::DataType &data);
};

#endif // MAINWINDOW_H
