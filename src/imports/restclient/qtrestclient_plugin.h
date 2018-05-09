#ifndef QTRESTCLIENT_PLUGIN_H
#define QTRESTCLIENT_PLUGIN_H

#include <QtQml/QQmlExtensionPlugin>

class QtRestClientDeclarativeModule : public QQmlExtensionPlugin
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID QQmlExtensionInterface_iid)

public:
	QtRestClientDeclarativeModule(QObject *parent = nullptr);
	void registerTypes(const char *uri) override;
};

#endif // QTRESTCLIENT_PLUGIN_H
