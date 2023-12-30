#ifndef PRISM_QT_CORE_HPP_PRISMMODELPROXY_H
#define PRISM_QT_CORE_HPP_PRISMMODELPROXY_H

#include "../prismQt_core_global.h"
#include "prism/prism.hpp"
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QVariant>
#include <memory>

namespace prism::qt::core
{

#if defined(__linux) || defined(__clang__)
template <class TV>
constexpr inline void get_field_do(QList<const char*>& fns, TV& p_v, QVariant& qv);
template <class TV>
constexpr inline bool set_field_do(QList<const char*>& fns, TV& p_v, QVariant& value);
template <class TV>
constexpr inline bool recurseSetValue(QList<const char*>& fns, TV& p_v, QVariant& value);
template <class TV>
constexpr inline void recurseGetValue(QList<const char*>& fns, TV& p_v, QVariant& qv);
#endif

template <class T>
struct has_def_indexer
{
    template <typename U>
    static auto test(U* p) -> decltype(p->operator[](0), std::false_type{});
    template <typename U>
    static auto test(...) -> std::true_type;
    static constexpr bool value = decltype(test<T>(nullptr))::value;
};

template <class T>
void getTransValue([[maybe_unused]] const char* fname, [[maybe_unused]] T& model, [[maybe_unused]] QVariant& qv)
{
}

class PRISMQT_CORE_EXPORT prismModelProxyBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool notifyFlag READ notifyFlag WRITE setNotifyFlag NOTIFY notifyFlagChanged)

    bool m_notifyFlag = false;

  public:
    virtual ~prismModelProxyBase()
    {
    }
    explicit prismModelProxyBase(QObject* parent = nullptr)
        : QObject(parent)
    {
        QQmlEngine::setObjectOwnership(this, QQmlEngine::ObjectOwnership::CppOwnership);
    }

    bool notifyFlag() const
    {
        return m_notifyFlag;
    }

  public slots:

    Q_INVOKABLE virtual QVariant get(const QByteArray& fname)
    {
        Q_UNUSED(fname)
        return QVariant();
    }
    Q_INVOKABLE virtual void set(const QByteArray& fname, QVariant value)
    {
        Q_UNUSED(fname)
        Q_UNUSED(value)
    }
    Q_INVOKABLE void update()
    {
        setNotifyFlag(!notifyFlag());
    }

    void setNotifyFlag(bool notifyFlag)
    {
        if (m_notifyFlag == notifyFlag)
            return;

        m_notifyFlag = notifyFlag;
        emit notifyFlagChanged(m_notifyFlag);
    }
  signals:
    void notifyFlagChanged(bool notifyFlag);
};

template <class T>
class prismModelProxy : public prismModelProxyBase
{
  public:
    using value_type = T;
    std::shared_ptr<T> instance()
    {
        return m_instance;
    }
    void setInstance(std::shared_ptr<T> newInstance)
    {
        m_instance = newInstance;
    }
    std::shared_ptr<T> m_instance = nullptr;
    virtual ~prismModelProxy<T>()
    {
    }
    prismModelProxy<T>(QObject* parent = nullptr, std::shared_ptr<T> instance = std::make_shared<T>())
        : prismModelProxyBase(parent), m_instance(instance)
    {
        QQmlEngine::setObjectOwnership(this, QQmlEngine::ObjectOwnership::CppOwnership);
    }

    // prismModelProxyBase interface
    QVariant get(const QByteArray& fname) override
    {
        QVariant qv;
        if (!m_instance)
            return qv;

        QList<QByteArray> fns = fname.simplified().replace(" ", "").split('.');
        QList<const char*> fns2;
        for (QByteArray& item : fns)
        {
            fns2.append(item);
        }

        recurseGetValue<T>(fns2, *m_instance.get(), qv);
        return qv;

        // for(const QByteArray& fn :fns)
        //{
        //     prism::reflection::field_do(*m_instance,fn.data(),[&](auto& v){
        //         qv = QVariant::fromValue(v);
        //     });
        // }
        // return qv;
    }

    void set(const QByteArray& fname, QVariant value) override
    {
        // qDebug()<< "set " << fname;
        if (!m_instance)
            return;

        QList<QByteArray> fns = fname.simplified().replace(" ", "").split('.');
        QList<const char*> fns2;
        for (QByteArray& item : fns)
        {
            fns2.append(item);
        }

        if (recurseSetValue<T>(fns2, *m_instance.get(), value))
            update();

        // prism::reflection::field_do(*m_instance,fname.data(),[&](auto& v){
        //     if(!value.canConvert<std::remove_reference_t< decltype(v)>>())
        //     {
        //         qDebug()<< "设置值失败,类型错误";
        //         return;
        //     }
        //     v =  value.value<std::remove_reference_t<decltype(v)>>();
        // });
    }
};

template <class TV>
constexpr inline void get_field_do(QList<const char*>& fns, TV& p_v, QVariant& qv)
{
    if (fns.length() == 0)
    {
        if constexpr (prism::utilities::is_specialization<TV, std::shared_ptr>::value ||
                      prism::utilities::is_specialization<TV, std::unique_ptr>::value)
        {
            qv = QVariant::fromValue(p_v.get());
        }
        else if constexpr (prism::utilities::is_specialization<TV, std::weak_ptr>::value)
        {
            qv = QVariant::fromValue(p_v.lock().get());
        }
        else if constexpr (prism::utilities::has_def<prism::enums::enum_info<TV>>::value)
        {
            const char* str = prism::enums::enum_info<TV>::tostring(p_v);
            qv = QVariant::fromValue(QString::fromStdString(str));
        }
        else if constexpr (std::is_same_v<std::string, TV>)
        {
            qv = QVariant::fromValue(QString::fromStdString(p_v));
        }
        else
        {
            qv = QVariant::fromValue(p_v);
        }
    }
    else
    {
        recurseGetValue(fns, p_v, qv);
    }
}

template <class TV>
constexpr inline bool set_field_do(QList<const char*>& fns, TV& p_v, QVariant& value)
{
    using model_t = std::remove_reference_t<decltype(p_v)>;
    if (fns.length() == 0)
    {
        if (!value.canConvert<model_t>())
        {
            qDebug() << "设置值失败,类型错误,不能把" << value << QString("设置给[%1]类型的变量").arg(QString::fromStdString(std::string(utilities::typeName<decltype(p_v)>::name())));
            return false;
        }
        auto v = value.value<model_t>();
        if (v != p_v)
        {
            p_v = v;
            return true;
        }
        else
            return false;
    }
    else
    {
        return recurseSetValue(fns, p_v, value);
    }
}
template <class TV>
constexpr inline bool recurseSetValue(QList<const char*>& fns, TV& p_v, QVariant& value)
{
    const char* fname = fns[0];
    fns.removeAt(0);
    bool isUpdate = false;
    if constexpr (std::is_pointer_v<TV> ||
                  prism::utilities::is_specialization<TV, std::shared_ptr>::value ||
                  prism::utilities::is_specialization<TV, std::unique_ptr>::value)
    {
        prism::reflection::field_do(*p_v, fname, [&](auto& v) {
            isUpdate |= set_field_do(fns, v, value);
        });
    }
    else if constexpr (prism::utilities::is_specialization<TV, std::weak_ptr>::value)
    {
        prism::reflection::field_do(*p_v.lock(), fname, [&](auto& v) {
            isUpdate |= set_field_do(fns, v, value);
        });
    }
    else if constexpr (prism::utilities::is_specialization<TV, std::optional>::value)
    {
        if (p_v.has_value())
        {
            prism::reflection::field_do(p_v.value(), fname, [&](auto& v) {
                isUpdate |= set_field_do(fns, v, value);
            });
        }
    }
    else
    {
        prism::reflection::field_do(p_v, fname, [&](auto& v) {
            isUpdate |= set_field_do(fns, v, value);
        });
    }

    return isUpdate;
}
template <class TV>
constexpr inline void recurseGetValue(QList<const char*>& fns, TV& p_v, QVariant& qv)
{
    const char* fname = fns[0];
    fns.removeAt(0);
    if constexpr (prism::utilities::is_specialization<TV, std::weak_ptr>::value)
    {
        using tt = std::remove_reference_t<std::remove_reference_t<decltype(*(p_v.lock()))>>;
        prism::reflection::field_do<0, tt>(*p_v.lock(), fname, [&](auto& v) {
            get_field_do(fns, v, qv);
        });
    }
    else if constexpr (std::is_pointer_v<TV> ||
                       prism::utilities::is_specialization<TV, std::shared_ptr>::value ||
                       prism::utilities::is_specialization<TV, std::unique_ptr>::value)
    {
        prism::reflection::field_do<0, std::remove_reference_t<std::remove_reference_t<decltype(*p_v)>>>(*p_v, fname, [&](auto& v) {
            get_field_do(fns, v, qv);
        });
    }
    else
    {
        prism::reflection::field_do(p_v, fname, [&](auto& v) {
            get_field_do(fns, v, qv);
        });
    }
}

} // namespace prism::qt::core

#endif // PRISM_QT_CORE_HPP_PRISMMODELPROXY_H
