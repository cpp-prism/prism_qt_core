#include "include/prism/qt/core/helper/sortfilterproxymodel.h"
#include "include/prism/qt/core/hpp/prismQt.hpp"
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

namespace prism::qt::core{
prismQt_core::prismQt_core()
{
}

bool prismQt_core::register_types()
{
    Container::get()->register_instance<prismQt_events>(std::make_shared<prismQt_events>());

    qRegisterMetaType<std::condition_variable*>("std::condition_variable*");

    qmlRegisterSingletonInstance<prismBind>("prismCpp", 1, 0, "Bind", new prismBind());
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
