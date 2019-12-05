#include "jphpost.h"

JphPost::JphPost(QObject *parent) :
	QObject{parent},
	id{0},
	userId{0},
	title{},
	body{}
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

JphPost *JphPost::createDefault(QObject *parent)
{
	return new JphPost{1, 1, QStringLiteral("Title1"), QStringLiteral("Body1"), parent};
}

JphPost *JphPost::createFirst(QObject *parent)
{
	return new JphPost{0, 0, QStringLiteral("Title0"), QStringLiteral("Body0"), parent};
}



JphPostSimple::JphPostSimple(QObject *parent) :
	Simple{parent}
{}
