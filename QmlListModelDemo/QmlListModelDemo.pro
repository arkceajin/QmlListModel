TEMPLATE = app

QT += qml quick

SOURCES += main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

HEADERS += \
    CompanyModel.h \
    QmlListModel.h \
    MemberModel.h

QMAKE_CXXFLAGS += -std=c++11
