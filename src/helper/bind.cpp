#include "bind.h"
#include <prism/container.hpp>

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

namespace prism::qt::core{
prismBind::prismBind(QObject *parent)
    : QObject{parent}
{

}

QJSValue prismBind::create(const QJSValue obj, const QString &expr)
{
    QString placehold = "prism_obj?(prism_obj.notifyFlag? prism_obj.get('$props$'):prism_obj.get('$props$')):null";
    QJSValue result ;

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    auto engine = obj.engine();
#else
    QJSEngine* engine = qjsEngine(this);
#endif

    if(engine)
    {
        try{
            engine->globalObject().setProperty("prism_obj",obj);
            result = engine->evaluate(placehold.replace("$props$", expr));
        }
        catch(...)
        {

        }
    }

    if (result.isError()) {
        qWarning() << "Error evaluating JavaScript expression:" << result.toString();
        //return QJSValue();
    }

    return result;
}
}// namespace prism::qt::core
