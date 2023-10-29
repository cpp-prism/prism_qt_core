#ifndef PRISM_QT_CORE_BIND_H
#define PRISM_QT_CORE_BIND_H

#include <prism/container.hpp>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QJSEngine>
#include <QJSValue>
#include <QDebug>

namespace prism::qt::core{

class prismBind : public QObject
{
    Q_OBJECT
public:
    prismBind(QObject* parent = nullptr);
public slots:
    QJSValue create(const QJSValue obj, const QString &expr);



signals:

};

}// namespace prism::qt::core
#endif // PRISM_QT_CORE_BIND_H
