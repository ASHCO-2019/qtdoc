TEMPLATE = app

QT += quick qml
SOURCES += main.cpp
RESOURCES += tweetsearch.qrc

OTHER_FILES = tweetsearch.qml \
              content/*.qml \
              content/*.mjs \
              content/resources/*

target.path = $$[QT_INSTALL_EXAMPLES]/demos/tweetsearch
INSTALLS += target
