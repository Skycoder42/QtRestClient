#include "simplejphpost.h"

SimpleJphPost::SimpleJphPost(QObject *parent) :
	Simple(parent),
	id(0),
	title(),
	href()
{}

SimpleJphPost::SimpleJphPost(int id, QString title, QUrl href, QObject *parent) :
	Simple(parent),
	id(id),
	title(title),
	href(href)
{}
