# QmlListModel
Expose a list of your customized C++ data structure to QML, easily access data from C++ and QML both sides.

## How to add QmlListModel in your project

  1. Put `QmlListModel.h` into your project, add it into `.pro `, and enable C++11 by adding `QMAKE_CXXFLAGS += -std=c++11`. 

  2. Create your data class which derivers from `QObject`.
  ```c++
  class Apartment : public QObject
  {
      Q_OBJECT
      Q_PROPERTY(QString name MEMBER mName)
  public:
      QString mName;
  }
  ```

  3. Create the list model class which derivers from `QmlListModel<Data>`,
  adding `Q_OBJECT` and `QML_LIST_MODEL` macro in the header of data model class.
  ```c++
  #include "QmlListModel.h"
  
  class CompanyModel : public QmlListModel<Apartment>
  {
      Q_OBJECT
      QML_LIST_MODEL
  };
  ```
  4. Registers module by [qmlRegisterType](http://doc.qt.io/qt-5/qqmlengine.html#qmlRegisterType). 
  
  ## Using in C++ side
  1. The QmlListModel provides `getData` `appendData` etc. functions to accessing the data list.
  
  2. Serialize and unserialize it into [QByteArray](http://doc.qt.io/qt-5/qbytearray.html) or `JSON`.
  
  ## Using in QML side
  1. Display data using [Repeater](http://doc.qt.io/qt-5/qml-qtquick-repeater.html) or [ListView](https://doc-snapshots.qt.io/qt5-5.9/qml-qtquick-listview.html)
  
  2. Access the data in JavaScript.
  
  ## Demo
  The QmlListModelDemo create a nested data structrue like this:
  ```
  CompanyModel
  ├── Apartment 1
  │   ├── Member A
  │   └── Member B
  ├── Apartment 2
  │   ├── Member C
  │   ├── Member D
  │   └── Member E
  └── Apartment 3
  ```
