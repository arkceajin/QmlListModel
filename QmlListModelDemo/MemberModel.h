#ifndef MEMBERMODEL_H
#define MEMBERMODEL_H

#include "QmlListModel.h"

class Member : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString memberName MEMBER mName)
public:
    explicit Member(){}
    explicit Member(QString aName): mName(aName){}

    QString mName;
};

class MemberModel : public QmlListModel<Member>
{
    Q_OBJECT
    QML_LIST_MODEL
public:
    explicit MemberModel(){}

    Q_INVOKABLE void addMember(QString name){
        appendData(new Member(name));
    }
};

#endif // MEMBERMODEL_H
