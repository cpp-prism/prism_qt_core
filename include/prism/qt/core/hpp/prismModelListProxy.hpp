#ifndef PRISM_QT_CORE_HPP_PRISMMODELLISTPROXY_HPP
#define PRISM_QT_CORE_HPP_PRISMMODELLISTPROXY_HPP
#include "../prismQt_core_global.h"
#include "prismModelProxy.hpp"
#include <QAbstractListModel>
#include <QCoreApplication>
#include <QObject>
#include <memory>
#include <prism/prism.hpp>
#include <type_traits>

namespace prism::qt::core
{

class PRISMQT_CORE_EXPORT prismModelListProxyBase : public QAbstractListModel
{
    Q_OBJECT
  public:
    prismModelListProxyBase(QObject* parent = nullptr)
    {
        Q_UNUSED(parent)
    }
    virtual ~prismModelListProxyBase()
    {
    }

    Q_INVOKABLE virtual void removeItemAt(int i)
    {
        Q_UNUSED(i)
    }
    Q_INVOKABLE virtual QVariant getRowData(int index)
    {
        Q_UNUSED(index)
        return QVariant();
    }
    Q_INVOKABLE virtual QVariant getRowData(const QModelIndex& index)
    {
        Q_UNUSED(index)
        return QVariant();
    }
    int size()
    {
        return this->length();
    }
    Q_INVOKABLE virtual int length()
    {
        return 0;
    }
    Q_INVOKABLE virtual int indexOfValue(QString fname, QVariant value)
    {
        Q_UNUSED(fname)
        Q_UNUSED(value)
        return 0;
    }
    Q_INVOKABLE virtual void pub_beginResetModel()
    {
    }
    Q_INVOKABLE virtual void pub_endResetModel()
    {
    }
};

template <typename T>
class prismModelListProxy : public prismModelListProxyBase
{

    QVector<std::shared_ptr<prismModelProxy<T>>>* m_list = nullptr;

  public:
    using value_type = T;
    explicit prismModelListProxy(QObject* parent = nullptr)
        : prismModelListProxyBase(parent), m_list(new QVector<std::shared_ptr<prismModelProxy<T>>>())
    {
        Q_UNUSED(parent)
        QQmlEngine::setObjectOwnership(this, QQmlEngine::ObjectOwnership::CppOwnership);
    }

    ~prismModelListProxy()
    {
        if (m_list)
        {
            delete m_list;
            m_list = nullptr;
        }
    }

    QVariant getRowData(const QModelIndex& index) override
    {
        if (!index.isValid())
            return QVariant();
        int idx = index.row();
        if (idx < 0)
            return QVariant();
        if (idx >= m_list->size())
            return QVariant();
        if (m_list)
        {
            std::shared_ptr<prismModelProxy<T>> tm = m_list->at(idx);
            return QVariant::fromValue(tm.get());
        }

        return QVariant();
    }
    QVariant getRowData(int index) override
    {
        if (index < 0)
            return QVariant();
        if (index >= m_list->size())
            return QVariant();
        if (m_list)
        {
            std::shared_ptr<prismModelProxy<T>> tm = m_list->at(index);
            return QVariant::fromValue(tm.get());
        }

        return QVariant();
    }

    QVector<std::shared_ptr<prismModelProxy<T>>>* list() const
    {
        return m_list;
    }

    // Basic functionality:
    int rowCount(const QModelIndex& parent = QModelIndex()) const override
    {
        if (parent.isValid() || !list())
            return 0;

        return list()->size();
    }

    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override
    {
        Q_UNUSED(role)
        if (!index.isValid() || !list())
            return QVariant();
        std::shared_ptr<prismModelProxy<T>> tm = m_list->at(index.row());

        // todo
        return tm->get(roleNames()[role].data());
    }

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override
    {
        if (!m_list || index.row() < 0 || index.row() + 1 > m_list->count())
            return false;
        std::shared_ptr<prismModelProxy<T>> tm = m_list->at(index.row());

        QVariant* constp = const_cast<QVariant*>(reinterpret_cast<const QVariant*>(&value));
        tm->set(roleNames()[role].data(), *constp);
        emit dataChanged(index, index, {role});
        return true;
    }

    bool setData(int index, QVariant& value, QByteArray roleName)
    {
        if (!m_list)
            return false;
        if (index < 0 || index + 1 > m_list->count())
            return false;

        prismModelProxy<T>* tm = m_list->at(index)->get();

        int role = nameRoles()[roleName];
        tm->set(roleName.data(), value);
        emit dataChanged(this->index(index, 0), this->index(index, 0), {role});
        return true;
    }

    Qt::ItemFlags flags(const QModelIndex& index) const override
    {
        if (!index.isValid())
            return Qt::NoItemFlags;

        return Qt::ItemIsEditable; // FIXME: Implement me!
    }

    QHash<int, QByteArray> roleNames() const override
    {
        static QHash<int, QByteArray> roles;
        if (roles.count() == 0)
        {
            QHash<int, QByteArray> roles2;
            int role1 = 500;
            prism::reflection::for_each_fields(*static_cast<T*>(nullptr), [&](const char* name, auto& v) {
                Q_UNUSED(v)
                roles2[role1++] = name;
            });
            roles = roles2;
        }
        return roles;
    }
    QHash<QByteArray, int> nameRoles() const
    {
        static QHash<QByteArray, int> names;
        static int role2;
        if (names.count() == 0)
        {
            role2 = 500;
            prism::reflection::for_each_fields(*static_cast<T*>(nullptr), [&](const char* name, auto& v) {
                Q_UNUSED(v)
                names[name] = role2++;
            });
        }
        return names;
    }
    virtual void appendItem(std::shared_ptr<T> tm)
    {
        beginInsertRows(QModelIndex(), m_list->size(), m_list->size());
        m_list->append(std::make_shared<prismModelProxy<T>>(this, tm));
        endInsertRows();
    }
    virtual void appendItemNotNotify(std::shared_ptr<T> tm)
    {
        m_list->append(std::make_shared<prismModelProxy<T>>(this, tm));
    }
    virtual void removeAllItemsNotNotify()
    {
        int total = m_list->size();
        for (int i = 0; i < total; i++)
        {
            QCoreApplication::processEvents();
            m_list->removeAt(0);
        }
    }
    virtual void removeAllItems()
    {
        int total = m_list->size();
        for (int i = 0; i < total; i++)
        {
            QCoreApplication::processEvents();
            beginRemoveRows(QModelIndex(), 0, 0);
            m_list->removeAt(0);
            endRemoveRows();
        }
    }
    virtual void removeItemAt(int i)
    {
        beginRemoveRows(QModelIndex(), i, i);
        m_list->removeAt(i);
        endRemoveRows();
    }

    int length() override
    {
        return m_list ? m_list->size() : 0;
    }
    int indexOfValue(QString fname, QVariant value) override
    {
        if (value.isNull() || !value.isValid() || list()->size() == 0)
            return -1;
        for (int i = 0; i < list()->size(); ++i)
        {
            prismModelProxy<T>* item = list()->at(i).get();
            if (value == item->get(fname.toStdString().data()))
            {
                return i;
            }
        }
        return -1;
    }

    // prismModelListProxyBase interface
  public:
    void pub_beginResetModel() override
    {
        this->beginResetModel();
    }
    void pub_endResetModel() override
    {
        this->endResetModel();
    }
};
} // namespace prism::qt::core
#endif // PRISM_QT_CORE_HPP_PRISMMODELLISTPROXY_HPP
