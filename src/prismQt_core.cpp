#include "include/prism/qt/core/helper/sortfilterproxymodel.h"
#include "include/prism/qt/core/hpp/prismQt.hpp"
#include "include/prism/qt/core/hpp/prismDataAccessLayer.hpp"
#include "include/prism/qt/core/prismQt_events.h"
#include "prismQt_core.h"
#include "src/helper/bind.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QGuiApplication>
#include <QIcon>
#include <QObject>
#include <QPluginLoader>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QVariant>
#include <prism/container.hpp>
#include <prism/prismJson.hpp>
#include <prism/qt/modular/interfaces/intf_module.h>
#include <condition_variable>
#include <QQmlEngine>
#include <QtQml>

namespace prism::qt::core{
prismQt_core::prismQt_core()
{
}

bool prismQt_core::register_types()
{
    Container::get()->register_instance<prismQt_events>(std::make_shared<prismQt_events>());

    qRegisterMetaType<std::condition_variable*>("std::condition_variable*");
    qRegisterMetaType<Sql_logic_base*>("Sql_logic_base*");
    qRegisterMetaType<bool*>("bool*");

    //注册单例 到qml engine  qt 5.15的语法
    //qmlRegisterSingletonInstance<prismBind>("prismCpp", 1, 0, "Bind", new prismBind());
    //注册单例 到qml engine  qt 5.12的  5.15亦可语法
    qmlRegisterSingletonType<prismBind>("prismCpp",1,0 ,"Bind",[](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
            Q_UNUSED(engine)
            Q_UNUSED(scriptEngine)
            return  new prismBind();
    });

    qmlRegisterType<SortFilterProxyModel>("prismCpp", 1, 0, "SortFilterProxyModelCpp");

    return true;
}

bool prismQt_core::init()
{
    std::shared_ptr<prismQt_events> eventhub = Container::get()->resolve_object<prismQt_events>();
    connect(eventhub.get(), &prismQt_events::clear_qml_cache, this, []() {
        std::shared_ptr<QQmlApplicationEngine> engine = Container::get()->resolve_object<QQmlApplicationEngine>();
        engine->clearComponentCache();
    });





    return true;
}

bool prismQt_core::install()
{
    return true;
}

bool prismQt_core::uninstall()
{
    return true;
}

bool prismQt_core::uninit()
{
    return true;
}

} // namespace prism::qt::core
