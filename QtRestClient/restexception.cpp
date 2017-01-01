#include "restexception.h"
using namespace QtRestClient;

Exception::Exception(const QString &what, int code) :
	std::exception(),
	error(what.toUtf8()),
	eCode(code)
{}

QString Exception::qWhat() const
{
	return QString::fromUtf8(this->error);
}

int Exception::code() const
{
	return this->eCode;
}

const char *Exception::what() const noexcept
{
	return this->error.constData();
}
