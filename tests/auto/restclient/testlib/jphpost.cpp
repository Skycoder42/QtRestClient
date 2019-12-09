#include "jphpost.h"

JphPost::JphPost(QObject *parent) :
	QObject{parent}
{}

JphPost::JphPost(int id, int userId, QString title, QString body, QObject *parent) :
	QObject{parent},
	id{id},
	userId{userId},
	title{std::move(title)},
	body{std::move(body)}
{}

bool JphPost::equals(const JphPost *left, const QObject *right)
{
	if (left) {
		if (!left->equals(right))
			return false;
	} else if (left != right)
		return false;
	return true;
}

bool JphPost::equals(const QObject *other) const
{
	if (this == other)
		return true;
	else if (!other)
		return false;
	else if (metaObject()->className() != other->metaObject()->className())
		return false;
	else {
		for (auto i = staticMetaObject.propertyOffset(); i < metaObject()->propertyCount(); i++) {
			auto property = metaObject()->property(i);
			QMetaType t(property.userType());
			if (t.flags().testFlag(QMetaType::PointerToQObject) &&
			   t.metaObject()->inherits(&staticMetaObject)) {
				auto c1 = property.read(this).value<JphPost*>();
				auto c2 = property.read(other).value<JphPost*>();
				if (!equals(c1, c2))
					return false;
			} else if (property.read(this) != property.read(other))
					return false;
		}
		return true;
	}
}

JphPost *JphPost::create(int index, QObject *parent)
{
	return new JphPost{
		index,
		qCeil(index/2.0),
		QStringLiteral("Title%1").arg(index),
		QStringLiteral("Body%1").arg(index),
		parent
	};
}

JphPost *JphPost::createDefault(QObject *parent)
{
	return create(1, parent);
}

JphPost *JphPost::createFirst(QObject *parent)
{
	return create(0, parent);
}



JphPostSimple::JphPostSimple(QObject *parent) :
	Simple{parent}
{}

JphPostSimple::JphPostSimple(int id, QString title, QUrl href, QObject *parent) :
	Simple{parent},
	id{id},
	title{std::move(title)},
	href{std::move(href)}
{}
