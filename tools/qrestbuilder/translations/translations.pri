TRANSLATIONS += $$PWD/qrestbuilder_de.ts \
	$$PWD/qrestbuilder_template.ts

OTHER_FILES += $$TRANSLATIONS

qtPrepareTool(LRELEASE, lrelease)

releaseTarget.target = lrelease
releaseTarget.commands = $$LRELEASE "$$_PRO_FILE_"
QMAKE_EXTRA_TARGETS += releaseTarget

trInstall.path = $$[QT_INSTALL_TRANSLATIONS]
trInstall.files = $$PWD/qrestbuilder_de.qm \
	$$PWD/qrestbuilder_template.ts
trInstall.CONFIG += no_check_exist
trInstall.depends = releaseTarget
INSTALLS += trInstall
