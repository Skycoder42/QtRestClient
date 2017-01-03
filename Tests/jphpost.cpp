#include "jphpost.h"

JphPost::JphPost(QObject *parent) :
	RestObject(parent),
	id(0),
	userId(0),
	title(),
	body()
{}

JphPost::JphPost(int id, int userId, QString title, QString body, QObject *parent) :
	RestObject(parent),
	id(id),
	userId(userId),
	title(title),
	body(body)
{}

JphPost *JphPost::createDefault(QObject *parent)
{
	return new JphPost(1, 1, "Title1", "Body1", parent);
}
