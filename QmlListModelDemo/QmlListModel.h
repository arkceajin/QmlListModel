#ifndef QMLLISTMODEL_H
#define QMLLISTMODEL_H

/**
  * Enable or Disable Serialize
  */
#define UsingSerialize  0

/**
  * Enable or Disable Json
  */
#define UsingJson       0

#include <QAbstractListModel>
#include <QMetaProperty>
#include <QQmlEngine>
#if UsingJson
    #include <QJsonDocument>
    #include <QJsonObject>
    #include <QJsonArray>
#endif
#if UsingSerialize
    #include <QDataStream>
#endif
#include <QDebug>

/**
 * @brief The QAbstractBase class
 * TBD
 */
class QAbstractBase : public QAbstractListModel
{
public:
    explicit QAbstractBase(QObject *parent = 0):
    QAbstractListModel(parent){}

#if UsingSerialize
    virtual void fromBytes(QDataStream& s){
        Q_UNUSED(s);
    }

    virtual void toBytes(QDataStream& s){
        Q_UNUSED(s);
    }

    virtual QByteArray serialize(){
        return QByteArray();
    }

    virtual void unserialize(QByteArray data) {
        Q_UNUSED(data);
    }
#endif

#if UsingJson
    virtual QJsonArray toJson(){
        return QJsonArray();
    }

    virtual bool fromJson(QJsonArray array){
        Q_UNUSED(array);
        return false;
    }

    virtual bool jsonToObj(const QJsonValue &jsonValue, QObject *obj){
        Q_UNUSED(jsonValue);
        Q_UNUSED(obj);
        return false;
    }
#endif
};

/**
  * API for Javascript side.
  * The data operation directly manipulates the pointer of object.
  */
#define QML_LIST_MODEL \
public: \
    Q_INVOKABLE inline static QVariant create(){return create_();} \
    Q_INVOKABLE inline QVariant get(int i){return get_(i);} \
    Q_INVOKABLE inline void append(QVariant data){append_(data);} \
    Q_INVOKABLE inline bool insert(int i, QVariant data){return insert_(i, data);} \
    Q_INVOKABLE inline bool set(int i, QVariant data){return set_(i, data);} \
    Q_INVOKABLE inline int size(){return mData.size(); } \
    Q_INVOKABLE inline bool isEmpty(){return mData.isEmpty(); } \
    Q_INVOKABLE inline bool remove(int i){return removeData(i);}

template<typename T>
/**
 * @brief The QmlListModel class is able to construct a C++ object list for both QML and C++ using.
 * @author Jiu
 */
class QmlListModel : public QAbstractBase
{
public:
    inline explicit QmlListModel(QObject *parent = 0);

    ~QmlListModel();
#if UsingSerialize
    inline explicit QmlListModel(QByteArray byteArray){
        unserialize(byteArray);
    }
    /**
     * @brief serialize
     * @return Serialized data
     */
    inline QByteArray serialize() override {
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream<<this;
        return buffer;
    }

    /**
     * @brief unserialize
     * @param data Serialized data
     */
    inline void unserialize(QByteArray data) override {
        if(!data.isEmpty()){
            clear();
            QDataStream stream(data);
            stream>>this;
        }
    }

    /**
     * @brief operator >>
     * @param s Output stream
     * @param data Destination
     * @return
     */
    friend QDataStream& operator>>(QDataStream& s, QmlListModel<T>* data)
    {
        data->fromBytes(s);
        return s;
    }

    /**
     * @brief operator <<
     * @param s Input stream
     * @param data Origin
     * @return
     */
    friend QDataStream& operator<<(QDataStream& s, QmlListModel<T>* data)
    {
        data->toBytes(s);
        return s;
    }
#endif

#if UsingJson
    inline explicit QmlListModel(QJsonArray jsonArray){
        fromJson(jsonArray);
    }

    void fromBytes(QDataStream& s) override;

    void toBytes(QDataStream& s) override;

    /**
     * @brief toJson
     * @return Json array
     */
    QJsonArray toJson() override;

    /**
     * @brief toJsonDoc
     * @return Json document
     */
    inline QJsonDocument toJsonDoc(){
        QJsonDocument doc;
        doc.setArray(toJson());
        return doc;
    }

    /**
     * @brief fromJson
     * @param doc Json document
     * @return the result of converion
     */
    inline bool fromJson(QJsonDocument doc){
        if(doc.isArray()){
            return fromJson(doc.array());
        } return false;
    }

    /**
     * @brief jsonToObj
     * @param jsonValue origin
     * @param obj destination
     * @return the result of converion
     */
    bool jsonToObj(const QJsonValue& jsonValue, QObject* obj) override;

    /**
     * @brief fromJson
     * @param array Json array
     * @return the result of converion
     */
    bool fromJson(QJsonArray array) override;
#endif
    /**
     * @brief clear
     */
    void clear();

    /**
     * @brief rowCount
     * @param parent
     * @return
     */
    int rowCount(const QModelIndex &parent) const override{
        Q_UNUSED(parent);
        return mData.count();
    }

    /**
     * @brief data
     * @param index
     * @param role
     * @return
     */
    QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const override;

    inline QVariant data(const int& i, const QByteArray& role) const {
        return data(index(i), roleNames().key(role));
    }

    static T* cloneData(const T* data);

    /**
     * @brief appendData
     * @param data
     */
    void appendData(T *data);
    /**
     * @brief getData
     * @param i
     * @return
     */
    T* getData(int i);
    /**
     * @brief insertData
     * @param i
     * @param data
     * @return
     */
    bool insertData(int i, T* data);
    /**
     * @brief setData
     * @param i
     * @param data
     * @return
     */
    bool setData(int i, T* data);

    /**
     * @brief removeData
     * @param i
     * @return
     */
    bool removeData(int i);

signals:

public slots:

protected:
    /**
     * @brief create_
     * @return
     */
    static QVariant create_();

    /**
     * @brief append_
     * @param data
     */
    inline void append_(QVariant data){
        appendData(data.value<T*>());
    }
    /**
     * @brief get_
     * @param i
     * @return
     */
    inline QVariant get_(int i){
        return QVariant::fromValue<T*>(getData(i));
    }
    /**
     * @brief insert_
     * @param i
     * @param data
     * @return
     */
    inline bool insert_(int i, QVariant data){
        return insertData(i, data.value<T*>());
    }
    /**
     * @brief set_
     * @param i
     * @param data
     * @return
     */
    inline bool set_(int i, QVariant data){
        return setData(i, data.value<T*>());
    }

    /**
     * @brief roleNames
     * @return
     */
    QHash<int, QByteArray> roleNames() const override;
    /**
     * @brief Data list
     */
    QList<T*> mData;

};

/**
  * Implementation
  */
template<typename T>
QmlListModel<T>::QmlListModel(QObject *parent) :
    QAbstractBase(parent)
{
}

template<typename T>
QmlListModel<T>::~QmlListModel()
{
    clear();
}

#if UsingSerialize
template<typename T>
void QmlListModel<T>::toBytes(QDataStream &s)
{
    const QList<T*>& l = mData;
    s << quint32(l.size());
    for (int i = 0; i < l.size(); ++i) {
        T* t = l.at(i);
        const QMetaObject* metaData = t->metaObject();
        for(int j = metaData->propertyOffset(); j < metaData->propertyCount(); ++j) {
            const QMetaProperty& p = metaData->property(j);
            const QVariant& v = p.read(t);
            if(QString(p.typeName()).endsWith('*')){
                s << reinterpret_cast<QmlListModel*>(qvariant_cast<QObject *>(p.read(t)));
            } else {
                s << v;
            }
        }
    }
}

template<typename T>
void QmlListModel<T>::fromBytes(QDataStream &s)
{
    QList<T*>& l = this->mData;
    l.clear();
    quint32 c;
    s >> c;
    l.reserve(c);
    for(quint32 i = 0; i < c; ++i) {
        T* t = new T;
        const QMetaObject* metaData = t->metaObject();
        for(int j = metaData->propertyOffset(); j < metaData->propertyCount(); ++j) {
            const QMetaProperty& p = metaData->property(j);
            if(QString(p.typeName()).endsWith('*')){
                QmlListModel* subList = reinterpret_cast<QmlListModel*>(qvariant_cast<QObject *>(p.read(t)));
                if(subList != Q_NULLPTR)
                    s >> subList;
                else
                    qDebug()<<"QmlListModel"<<__FUNCTION__<<"Error: Null property.";
                    //subList = new QmlListModel; // TBD
            } else {
                QVariant v;
                s >> v;
                p.write(t, v);
            }
        }
        appendData(t);
        if (s.atEnd())
            break;
    }
}
#endif

#if UsingJson
template<typename T>
QJsonArray QmlListModel<T>::toJson()
{
    QJsonArray jsonArray;
    foreach (T* t, mData) {
        QJsonObject jsonObj;
        const QMetaObject* metaData = t->metaObject();
        for(int i = metaData->propertyOffset(); i < metaData->propertyCount(); ++i) {
            const QMetaProperty& p = metaData->property(i);
            const QVariant& v = p.read(t);
            if(QString(p.typeName()).endsWith('*')){
                QmlListModel* subList = reinterpret_cast<QmlListModel*>(qvariant_cast<QObject *>(p.read(t)));
                if(subList != Q_NULLPTR){
                    jsonObj.insert(p.name(), subList->toJson());
                } else {
                    jsonObj.insert(p.name(), QJsonValue());
                }
            } else {
                switch (p.type()) {
                case QVariant::Bool:
                    jsonObj.insert(p.name(), v.toBool());
                    break;
                case QVariant::Double:
                    jsonObj.insert(p.name(), v.toDouble());
                    break;
                case QVariant::UInt:
                case QVariant::Int:
                    jsonObj.insert(p.name(), v.toInt());
                    break;
                case QVariant::Char:
                case QVariant::String:
                    jsonObj.insert(p.name(), v.toString());
                    break;
                default:
                    qDebug()<<"QmlListModel"<<__FUNCTION__<<"Error: Wrong property."<<p.typeName()<<p.name()<<v;
                    break;
                }
            }
        }
        jsonArray.append(jsonObj);
    }
    return jsonArray;
}

template<typename T>
bool QmlListModel<T>::jsonToObj(const QJsonValue &jsonValue, QObject *obj)
{
    if(!jsonValue.isObject() || obj == Q_NULLPTR){
        return false;
    }
    const QJsonObject& jsonObj = jsonValue.toObject();
    const QMetaObject* metaData = obj->metaObject();
    if(jsonObj.size() != (metaData->propertyCount() - metaData->propertyOffset())){
        return false;
    }
    for(int i = metaData->propertyOffset(); i < metaData->propertyCount(); ++i) {
        const QMetaProperty& p = metaData->property(i);
        const QJsonValue& v = jsonObj.value(p.name());
        if(QString(p.typeName()).endsWith('*')){
            QmlListModel* subList = reinterpret_cast<QmlListModel*>(qvariant_cast<QObject *>(p.read(obj)));;
            if(v.isArray()){
                if(subList != Q_NULLPTR){
                    if(!subList->fromJson(v.toArray()))
                        return false;
                } else {
                    //subList = new QmlListModel(v.toArray()); // TBD
                    qDebug()<<"QmlListModel"<<__FUNCTION__<<"Error: Null property.";
                    return false;
                }
            } else if(v.isNull()){
                if(subList != Q_NULLPTR)
                    subList->deleteLater();
            } else {
                qDebug()<<"QmlListModel"<<__FUNCTION__<<"Error: Wrong property type."<<p.typeName()<<p.name()<<v;
                return false;
            }
        } else {
             if(!p.write(obj, QVariant(v))){
                qDebug()<<"QmlListModel"<<__FUNCTION__<<"Error: Write property failed."<<p.typeName()<<p.name()<<v;
                return false;
             }
        }
    }
    return true;
}

template<typename T>
bool QmlListModel<T>::fromJson(QJsonArray array)
{
    int i,
        mid = qMin(mData.size(), array.size()),
        max = qMax(mData.size(), array.size());
    /**
      * Replace the exists
      */
    for(i = 0; i < mid; i++){
        if(!jsonToObj(array.at(i), mData[i])){
            return false;
        }
    }
    if(array.size() < mData.size()){
        /**
          * Remove the out of bounds
          */
        for(i = mid; i < max; i++){
            if(!removeData(i)){
                return false;
            }
        }
    } else {
        for(i = mid; i < max; i++){
            T* d = new T;
            if(jsonToObj(array.at(i), d)){
                appendData(d);
            } else {
                d->deleteLater();
                qDebug()<<"QmlListModel"<<__FUNCTION__<<"Error: Append failed."<<i<<T::staticMetaObject.className();
                return false;
            }
        }
    }
    return true;
}
#endif

template<typename T>
void QmlListModel<T>::clear()
{
    beginRemoveRows(QModelIndex(), 0, mData.size());
    for(T* d : mData){
        d->deleteLater();
    }
    mData.clear();
    endRemoveRows();
}

template<typename T>
QVariant QmlListModel<T>::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mData.count())
        return QVariant();
    const T* data = mData[index.row()];
    return data->metaObject()->property(role).read(data);
}

template<typename T>
T *QmlListModel<T>::cloneData(const T *data)
{
    T* copy = new T;
    for(int i = T::staticMetaObject.propertyOffset(); i < T::staticMetaObject.propertyCount(); ++i) {
        T::staticMetaObject.property(i).write(copy, T::staticMetaObject.property(i).read(data));
    }
    return copy;
}

template<typename T>
void QmlListModel<T>::appendData(T *data)
{
    beginInsertRows(QModelIndex(), mData.count(), mData.count());
    QQmlEngine::setObjectOwnership(data, QQmlEngine::CppOwnership);
    mData.push_back(data);
    endInsertRows();
}

template<typename T>
T *QmlListModel<T>::getData(int i)
{
    if (i < 0 || i >= mData.count())
        return Q_NULLPTR;
    return mData[i];
}

template<typename T>
bool QmlListModel<T>::insertData(int i, T *data)
{
    if (i < 0 || i > mData.count())
        return false;
    beginInsertRows(QModelIndex(), i, i);
    QQmlEngine::setObjectOwnership(data, QQmlEngine::CppOwnership);
    mData.insert(mData.begin() + i, data);
    endInsertRows();
    return true;
}

template<typename T>
QVariant QmlListModel<T>::create_()
{
    T* data = new T;
    QQmlEngine::setObjectOwnership(data, QQmlEngine::CppOwnership);
    return QVariant::fromValue<T*>(data);
}

template<typename T>
bool QmlListModel<T>::setData(int i, T* data)
{
    if (i < 0 || i >= mData.count())
        return false;
    if (mData[i] == Q_NULLPTR)
        return false;
    mData[i]->deleteLater();
    QQmlEngine::setObjectOwnership(data, QQmlEngine::CppOwnership);
    mData[i] = data;
    dataChanged(index(i), index(i));
    return true;
}

template<typename T>
bool QmlListModel<T>::removeData(int i)
{
    if (i < 0 || i >= mData.count())
        return false;
    if (mData[i] == Q_NULLPTR)
        return false;
    mData[i]->deleteLater();
    beginRemoveRows(QModelIndex(), i, i);
    mData.erase(mData.begin() + i);
    endRemoveRows();
    return true;
}

template<typename T>
QHash<int, QByteArray> QmlListModel<T>::roleNames() const
{
    QHash<int, QByteArray> roles;
    for(int i = T::staticMetaObject.propertyOffset(); i < T::staticMetaObject.propertyCount(); ++i) {
        roles[i] = QByteArray(T::staticMetaObject.property(i).name());
    }
    return roles;
}

#endif // QMLLISTMODEL_H
