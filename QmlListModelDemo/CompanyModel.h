#ifndef COMPANYMODEL_H
#define COMPANYMODEL_H

#include "MemberModel.h"

class Apartment : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString apartmentName MEMBER mName)
    Q_PROPERTY(MemberModel* members MEMBER mMembers)
public:
    explicit Apartment(){}
    explicit Apartment(QString aName,
                       MemberModel* aMembers):
        mName(aName),
        mMembers(aMembers){}

    QString         mName;
    MemberModel*    mMembers;
};

class CompanyModel : public QmlListModel<Apartment>
{
    Q_OBJECT
    QML_LIST_MODEL
public:
    explicit CompanyModel(){}
};

#endif // COMPANYMODEL_H
