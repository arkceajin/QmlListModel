#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include "CompanyModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    MemberModel* members1 = new MemberModel();
    members1->appendData(new Member("Member A"));
    members1->appendData(new Member("Member B"));

    MemberModel* members2 = new MemberModel();
    members2->appendData(new Member("Member C"));
    members2->appendData(new Member("Member D"));
    members2->appendData(new Member("Member E"));

    CompanyModel* companyModel = new CompanyModel;
    companyModel->appendData(new Apartment("Apartment 1", members1));
    companyModel->appendData(new Apartment("Apartment 2", members2));

    qmlRegisterType<CompanyModel>("QmlListModel", 1, 0 , "CompanyModel");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("companyModel", companyModel);
    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    return app.exec();
}
