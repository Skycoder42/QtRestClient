#ifndef RESTEXCEPTION_H
#define RESTEXCEPTION_H

#include "qtrestclient_global.h"
#include <exception>
#include <QString>

namespace QtRestClient {

class Exception : public std::exception
{
public:
	//! Creates a new Exception from an error messsage and an error code
	Exception(const QString &what = QString(), int code = 0);

	//! Returns the error string of the exception
	QString qWhat() const;
	//! Returns the error code of the exception
	int code() const;
	//! Returns the error string of the exception as charactar array
	const char *what() const noexcept final;

private:
	const QByteArray error;
	const int eCode;
};

}

#endif // RESTEXCEPTION_H
