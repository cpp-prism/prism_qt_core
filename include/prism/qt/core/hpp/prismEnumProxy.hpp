#ifndef PRISMENUMPROXY_HPP
#define PRISMENUMPROXY_HPP

#include "../prismQt_core_global.h"
#include <QObject>
#include <QString>
#include <prism/prism.hpp>
#include <QQmlEngine>
namespace prism::qt::core{

class PRISMQT_CORE_EXPORT prismEnumProxyBase : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QString valueStr READ valueStr WRITE setValueStr NOTIFY valueStrChanged)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged)

public:
    explicit prismEnumProxyBase(QObject* parent = nullptr):QObject(parent)
    {
        QQmlEngine::setObjectOwnership(this, QQmlEngine::ObjectOwnership::CppOwnership);
    }

    const QStringList &options() const
    {
        return m_options;
    }
    void setOptions(const QStringList &newOptions)
    {
        if (m_options == newOptions)
            return;
        m_options = newOptions;
        emit optionsChanged();
    }

    const QString &valueStr() const
    {
        return m_valueStr;
    }
    virtual void setValueStr(const QString &newValueStr)
    {
        if (m_valueStr == newValueStr)
            return;
        m_valueStr = newValueStr;
        emit valueStrChanged();
    }

    int value() const
    {
        return m_value;
    }
    virtual void setValue(int newValue)
    {
        if (m_value == newValue)
            return;
        m_value = newValue;
        emit valueChanged();
    }

    virtual int qstr2int(QString svalue){Q_UNUSED(svalue) return 0;}
    virtual QString int2qstr(int ivalue){ Q_UNUSED(ivalue) return QString();}


    QStringList m_options {};
    QString m_valueStr = "";
    int m_value = 0;

signals:
    void optionsChanged();

    void valueStrChanged();

    void valueChanged();

private:
};



template<class T>
class prismEnumProxy : public prismEnumProxyBase
{
public:
    T getEnumValue()
    {
        return static_cast<T>(value());
    }
    virtual int qstr2int(QString svalue)
    {
        if constexpr (prism::utilities::has_def<prism::enums::enum_info<T>>::value)
                return static_cast<int>(prism::enums::enum_info<T>::fromstring(svalue.toStdString().c_str()));
        else
            return -1;
    }
    virtual QString int2qstr(int ivalue)
    {
        if constexpr (prism::utilities::has_def<prism::enums::enum_info<T>>::value)
                return QString::fromStdString(std::string(prism::enums::enum_info<T>::tostring(static_cast<T>(ivalue))));
        else
            return QString();
    }
    prismEnumProxy(QObject* parent = nullptr):prismEnumProxyBase(parent)
    {
        QQmlEngine::setObjectOwnership(this, QQmlEngine::ObjectOwnership::CppOwnership);
        if constexpr (prism::utilities::has_def<prism::enums::enum_info<T>>::value)
        {
            static QStringList qops ;
            if(!qops.size())
            {
                std::vector<std::string>& ops = prism::enums::enum_info<T>::getStrVector();
                for(const std::string& item : ops)
                {
                    qops.append(QString::fromStdString(item));
                }
            }
            setOptions(qops);
            setValueStr(qops.at(m_value));
        }
    }
    void setValue(int newValue) override
    {
        if (m_value == newValue)
            return;
        m_value = newValue;
        emit valueChanged();
        if(newValue>=0 && newValue < options().size())
        {
            QString newstr = options().at(newValue);
            setValueStr(newstr);
        }
    }

    void setValueStr(const QString &newValueStr) override
    {
        if (m_valueStr == newValueStr)
            return;
        m_valueStr = newValueStr;
        emit valueStrChanged();
        int idx = options().indexOf(newValueStr);
        if(idx >=0 && idx < options().size())
        {
            setValue(idx);
        }
    }
};



} //namespace prism::qt::core

#endif // PRISMENUMPROXY_HPP
