TEMPLATE = subdirs
QT_FOR_CONFIG += core network jsonserializer

SUBDIRS += \
	SimpleRestClientApp

!no_json_serializer {
	SUBDIRS += \
		#JsonPlaceholderApi
}
