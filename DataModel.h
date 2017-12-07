#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QAbstractListModel>
#include <QMetaProperty>
#include <QQmlEngine>

/**
 * @brief The QAbstractBase class
 * TBD
 */
class QAbstractBase : public QAbstractListModel
{
public:
    explicit QAbstractBase(QObject *parent = 0):
    QAbstractListModel(parent){}
};

#define QML_DATA_OBJECT \
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
 * @brief The DataModel class is able to construct a C++ object list for both QML and C++ using.
 * @author Jiu
 */
class DataModel : public QAbstractBase
{
public:
    inline explicit DataModel(QObject *parent = 0);
    ~DataModel();

    /**
     * @brief serialize
     * @return Serialized data
     */
    QByteArray serialize(){
        QByteArray buffer;
        QDataStream stream(&buffer, QIODevice::WriteOnly);
        stream<<mData;
        return buffer;
    }

    /**
     * @brief unserialize
     * @param data Serialized data
     */
    void unserialize(QByteArray data){
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
    friend QDataStream& operator>>(QDataStream& s, DataModel<T>* data)
    {
        QList<T*>& l = data->mData;
        l.clear();
        quint32 c;
        s >> c;
        l.reserve(c);
        for(quint32 i = 0; i < c; ++i)
        {
            T* t = new T;
            s >> t;
            //l.append(t);
            data->appendData(t);
            if (s.atEnd())
                break;
        }
        return s;
    }

    /**
     * @brief operator <<
     * @param s Input stream
     * @param data Origin
     * @return
     */
    friend QDataStream& operator<<(QDataStream& s, const DataModel<T>* data)
    {
        const QList<T*>& l = data->mData;
        s << quint32(l.size());
        for (int i = 0; i < l.size(); ++i)
            s << l.at(i);
        return s;
    }

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

signals:

public slots:

protected:
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
DataModel<T>::DataModel(QObject *parent) :
    QAbstractBase(parent)
{
}

template<typename T>
DataModel<T>::~DataModel()
{
    clear();
}

template<typename T>
void DataModel<T>::clear()
{
    beginRemoveRows(QModelIndex(), 0, mData.size());
    for(T* d : mData){
        d->deleteLater();
    }
    mData.clear();
    endRemoveRows();
}

template<typename T>
QVariant DataModel<T>::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= mData.count())
        return QVariant();
    const T* data = mData[index.row()];
    return data->metaObject()->property(role).read(data);
}

template<typename T>
void DataModel<T>::appendData(T *data)
{
    beginInsertRows(QModelIndex(), mData.count(), mData.count());
    mData.push_back(data);
    endInsertRows();
}

template<typename T>
T *DataModel<T>::getData(int i)
{
    if (i < 0 || i >= mData.count())
        return Q_NULLPTR;
    return mData[i];
}

template<typename T>
bool DataModel<T>::insertData(int i, T *data)
{
    if (i < 0 || i > mData.count())
        return false;
    beginInsertRows(QModelIndex(), i, i);
    mData.insert(mData.begin() + i, data);
    endInsertRows();
    return true;
}

template<typename T>
QVariant DataModel<T>::create_()
{
    T* data = new T;
    QQmlEngine::setObjectOwnership(data, QQmlEngine::CppOwnership);
    return QVariant::fromValue<T*>(data);
}

template<typename T>
bool DataModel<T>::setData(int i, T* data)
{
    if (i < 0 || i >= mData.count())
        return false;
    if (mData[i] == Q_NULLPTR)
        return false;
    mData[i]->deleteLater();
    mData[i] = data;
    dataChanged(index(i), index(i));
    return true;
}

template<typename T>
bool DataModel<T>::removeData(int i)
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
QHash<int, QByteArray> DataModel<T>::roleNames() const
{
    QHash<int, QByteArray> roles;
    for(int i = T::staticMetaObject.propertyOffset(); i < T::staticMetaObject.propertyCount(); ++i) {
        roles[i] = QByteArray(T::staticMetaObject.property(i).name());
    }
    return roles;
}

#endif // DATAMODEL_H
