#ifndef PRISM_QT_CORE_HPP_PRISMTREEMODEL_H
#define PRISM_QT_CORE_HPP_PRISMTREEMODEL_H

#include "../prismQt_core_global.h"
#include "prismTreeNodeProxy.hpp"
#include <QVariant>
#include <QObject>
#include <QAbstractItemModel>
#include <QQmlEngine>
#include <QModelIndex>

namespace prism::qt::core{
class  PRISMQT_CORE_EXPORT prismTreeModelProxyBase : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit prismTreeModelProxyBase(QObject *parent = nullptr)
    {
        QQmlEngine::setObjectOwnership(this,QQmlEngine::ObjectOwnership::CppOwnership);
        Q_UNUSED(parent)
    }
    virtual ~prismTreeModelProxyBase() {}
    Q_INVOKABLE virtual  QVariant getRowData(const QModelIndex& index) { Q_UNUSED(index) return QVariant(); }
};

template<class T>
class  prismTreeModelProxy : public prismTreeModelProxyBase
{
    std::shared_ptr<prismTreeNodeProxy<T>> m_rootNode =nullptr;
private:
    void privatee_recurseNodeDo(std::shared_ptr<prismTreeNodeProxy<T>> node,std::function<void(std::shared_ptr<prismTreeNodeProxy<T>>)>&& func)
    {
        if(!node)
            return;
        if(node->childCount())
        {
            for(std::shared_ptr<prismTreeNodeProxy<T>> child: node->childItems())
            {
                privatee_recurseNodeDo(child,std::move(func));
            }
        }
        func(node);

    }

public:
    using value_type =T;
    explicit prismTreeModelProxy<T>(QObject *parent = nullptr)
    {
        Q_UNUSED(parent)
        QQmlEngine::setObjectOwnership(this,QQmlEngine::ObjectOwnership::CppOwnership);
    }
    ~prismTreeModelProxy() {}
    QVariant getRowData(const QModelIndex& index)
    {
        if(index.isValid())
        {
            prismTreeNodeProxy<T>* rvm =  static_cast<prismTreeNodeProxy<T>*>(index.internalPointer());
            QQmlEngine::setObjectOwnership(rvm,QQmlEngine::ObjectOwnership::CppOwnership);
            if(rvm)
                return QVariant::fromValue(rvm);
        }
        return QVariant();
    }
    void  setRootNode(std::shared_ptr<prismTreeNodeProxy<T>> rootnode)
    {
        this->beginResetModel();

        m_rootNode = rootnode;
        QQmlEngine::setObjectOwnership(rootnode.get(),QQmlEngine::ObjectOwnership::CppOwnership);
        this->endResetModel();
    }
    std::shared_ptr<prismTreeNodeProxy<T>> rootNode()const
    {
        return m_rootNode;
    }

    int columnCount(const QModelIndex &parent) const override
    {
        Q_UNUSED(parent)
        int i=0;
        prism::reflection::for_each_fields(*(T*)nullptr,[&](const char* fname,auto&& value) {
            Q_UNUSED(fname)
            Q_UNUSED(value)
            i++;
        });
        return i;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if(!index.isValid())
            return QVariant();
        prismTreeNodeProxy<T>* item =   static_cast<prismTreeNodeProxy<T>*>(index.internalPointer());
        QVariant qv;
        const char* fname =roleNames()[role].data();
        if(item)
        {
            prism::reflection::field_do(*item,fname,[&](auto&& value){
                qv = QVariant::fromValue(value);
            });
        }

        return qv;

    }
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override
    {

        if (!hasIndex(row, column, parent))
            return QModelIndex();

        prismTreeNodeProxy<T>* parentItem;
        if(parent.isValid())
            parentItem = static_cast<prismTreeNodeProxy<T>*>(parent.internalPointer());
        else
            parentItem  = rootNode().get();

        prismTreeNodeProxy<T>* childItem = parentItem->child(row).get();
        if(childItem)
            return createIndex(row,column,childItem);
        return QModelIndex();
    }


    QModelIndex parent(const QModelIndex &index) const override
    {
        if(!index.isValid())
            return QModelIndex();
        prismTreeNodeProxy<T>* childItem = static_cast<prismTreeNodeProxy<T>*>(index.internalPointer());
        std::shared_ptr<prismTreeNodeProxy<T>> parentItem = childItem->parentItem();
        if(parentItem == rootNode())
            return QModelIndex();
        return createIndex(parentItem->row(), 0, parentItem.get());
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override
    {
        prismTreeNodeProxy<T>* parentItem = nullptr;
        if (parent.column() > 0)
            return 0;

        if (!parent.isValid())
            parentItem = rootNode().get();
        else
            parentItem = static_cast<prismTreeNodeProxy<T>*>(parent.internalPointer());

        if(parentItem)
            return parentItem->childCount();
        else
            return 0;
    }

    QHash<int,QByteArray> roleNames() const override
    {
        static QHash<int, QByteArray> roles;
        if (roles.count() == 0)
        {
            QHash<int,QByteArray> roles2;
            int role1 = 500;
            prism::reflection::for_each_fields(*static_cast<T*>(nullptr),[&](const char* name,auto&v){
                Q_UNUSED(v)
                roles2[role1++] = name;

            });
            roles = roles2;
        }
        return roles;
    }
    QHash<QByteArray,int> nameRoles()
    {
        static QHash<QByteArray, int> names;
        static int role2;
        if (names.count() == 0)
        {
            role2 = 500;
            prism::reflection::for_each_fields(*static_cast<T*>(nullptr),[&](const char* name,auto& v)
            {
                Q_UNUSED(v)
                names[name] = role2++ ;
            });
        }
        return names;
    }

    void pub_beginResetModel()
    {
        this->beginResetModel();
    }
    void pub_endResetModel()
    {
        this->endResetModel();
    }

    void recurseNodeDo(std::function<void(std::shared_ptr<prismTreeNodeProxy<T>>)>&& func)
    {
        if(rootNode())
        {
            privatee_recurseNodeDo(rootNode(),std::move(func));
        }

    }
};
}// namespace prism::qt::core
#endif // PRISM_QT_CORE_HPP_PRISMTREEMODEL_H
