#ifndef PRISM_QT_CORE_EVENTS_H
#define PRISM_QT_CORE_EVENTS_H

#include "prismQt_core_global.h"
#include <QObject>
#include <QQuickWindow>
#include <QEventLoop>

namespace prism::qt::core{

class PRISMQT_CORE_EXPORT prismQt_events : public QObject
{
    Q_OBJECT
  public:
    prismQt_events(QObject* parent = nullptr)
        : QObject(parent)
    {
    }
  signals:
    void clear_qml_cache();
};

}// namespace prism::qt::core
#endif // PRISM_QT_CORE_EVENTS_H
