#include "bind.h"
#pragma warning(disable: 4996)

namespace prism::qt::core{
prismBind::prismBind(QObject *parent)
    : QObject{parent}
{

}

QJSValue prismBind::create(const QJSValue obj, const QString &expr)
{
    QString placehold = "prism_obj?(prism_obj.notifyFlag? prism_obj.get('$props$'):prism_obj.get('$props$')):null";
    QJSValue result ;
    if(obj.engine())
    {
        try{
            obj.engine()->globalObject().setProperty("prism_obj",obj);
            result = obj.engine()->evaluate(placehold.replace("$props$", expr));
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
