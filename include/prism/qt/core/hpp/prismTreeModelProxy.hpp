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
    Q_INVOKABLE virtual QVariant getRootNode() { return QVariant();}
    Q_INVOKABLE virtual QVariant getRowData(const QModelIndex& index) { Q_UNUSED(index) return QVariant(); }
    Q_INVOKABLE virtual QVariant getRowDataSptr(const QModelIndex& index) { Q_UNUSED(index) return QVariant(); }
    Q_INVOKABLE virtual QModelIndex getIdexByData(QVariant) {return QModelIndex();}
    Q_INVOKABLE virtual bool removeNode(const QModelIndex& index) { Q_UNUSED(index) return false; }
    Q_INVOKABLE virtual bool removeNode(QVariant to_del_node) { Q_UNUSED(to_del_node) return false; }
    Q_INVOKABLE virtual bool removeNodeAllchildren(QVariant to_del_node) { Q_UNUSED(to_del_node) return false; }
    Q_INVOKABLE virtual bool insertNode(const QModelIndex& parent,int row,QVariant vnode){Q_UNUSED(parent) Q_UNUSED(row)  Q_UNUSED(vnode) return false;}

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

    QVariant getRootNode() {
        if(rootNode())
            return QVariant::fromValue(rootNode().get());
        else
            return QVariant();
    }

    QModelIndex recurseGetIndex(void* data = nullptr,  QModelIndex parent = QModelIndex())
    {
        if (!parent.isValid())
        {
            parent = this->createIndex(0,0,rootNode().get());
        }
        void* internalPointer = parent.internalPointer();
        if (internalPointer == data)
        {
            return parent;
        }
        else
        {
            int rowCount = this->rowCount(parent);
            for (int row = 0; row < rowCount; ++row)
            {
                QModelIndex index = this->index(row, 0, parent);
                QModelIndex result = recurseGetIndex(data, index);
                if (result.isValid())
                    return result;  // 如果找到匹配项，则返回
            }
        }
        return QModelIndex();  // 如果未找到匹配项，返回无效的 QModelIndex
    }

    void traverseModel(void* to_del_node=nullptr, const QModelIndex &parent = QModelIndex())
    {
        int rowCount = this->rowCount(parent);

        for (int row = 0; row < rowCount; ++row)
        {
            QModelIndex index = this->index(row, 0, parent);

            void* internalPointer =   index.internalPointer();
            if(internalPointer == to_del_node)
            {
                this->removeNode(index);
                return;
            }

            if (this->hasChildren(index)) {
                traverseModel(to_del_node, index);
            }
        }
    }
public:
    using value_type =T;
    explicit prismTreeModelProxy<T>(QObject *parent = nullptr)
    {
        Q_UNUSED(parent)
        QQmlEngine::setObjectOwnership(this,QQmlEngine::ObjectOwnership::CppOwnership);
    }
    ~prismTreeModelProxy() {}
    QVariant getRowData(const QModelIndex& index) override
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
    QVariant getRowDataSptr(const QModelIndex& index) override
    {
        prismTreeNodeProxy<T>* parentItem;
        if(index.parent().isValid())
            parentItem = static_cast<prismTreeNodeProxy<T>*>(index.parent().internalPointer());
        else
            parentItem  = rootNode().get();

        std::shared_ptr<prismTreeNodeProxy<T>> result = parentItem->child(index.row());
        return QVariant::fromValue(result);
    }
    void  setRootNode(std::shared_ptr<prismTreeNodeProxy<T>> rootnode)
    {
        this->beginResetModel();

        m_rootNode = rootnode;
        QQmlEngine::setObjectOwnership(rootnode.get(),QQmlEngine::ObjectOwnership::CppOwnership);
        this->endResetModel();
    }
    void recoverRelationship(std::shared_ptr<prismTreeNodeProxy<T>> node)
    {
        for(std::shared_ptr<prismTreeNodeProxy<T>> child: node->m_childItems)
        {
            QQmlEngine::setObjectOwnership(child.get(),QQmlEngine::ObjectOwnership::CppOwnership);
            child->setParentItem(node);
            recoverRelationship(child);
        }
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
        //if (parent.column() > 0)
        //    return 0;

        if (!parent.isValid())
            parentItem = rootNode().get();
        else
            parentItem = static_cast<prismTreeNodeProxy<T>*>(parent.internalPointer());

        if(parentItem)
            return parentItem->childCount();
        else
            return 0;
    }

    bool insertNode(const QModelIndex& parent,int row,QVariant vnode) override
    {
        std::shared_ptr<prismTreeNodeProxy<T>> node = vnode.value<std::shared_ptr<prismTreeNodeProxy<T>>>() ;
        prismTreeNodeProxy<T>* parentItem;
        if(parent.isValid())
            parentItem = static_cast<prismTreeNodeProxy<T>*>(parent.internalPointer());
        else
            parentItem  = rootNode().get();

        if(row <0)
        {
            qDebug()<< "insert index out of range:" <<row;
            return false;
        }

        if(row > static_cast<int>(parentItem->m_childItems.size()))
        {
            row = static_cast<int>(parentItem->m_childItems.size());
            //qDebug()<< "insert index out of range :" <<row;
            //return false;
        }

        beginInsertRows(parent, row, row);
        parentItem->m_childItems.insert(parentItem->m_childItems.begin()+row,node);
        endInsertRows();
        return true;
    }

    QModelIndex getIdexByData(QVariant data) override
    {
        void* p = *static_cast<void**>(data.data());
        return recurseGetIndex(p,QModelIndex());
    }
    bool removeNode(QVariant to_del_node) override
    {
        void* p = *static_cast<void**>(to_del_node.data());
        traverseModel(p);
        return true;
    }
    bool removeNode(const QModelIndex& index) override
    {
        if(!index.isValid())
        {
            qDebug()<< "try to remove an invalid qmodelindex";
            return false;
        }

        ////递归删除子节点
        //int rowCount = this->rowCount(index);
        //for (int row = 0; row < rowCount; ++row)
        //{
        //    QModelIndex childindex = this->index(row, 0, index);
        //    removeNode(childindex);
        //}


        prismTreeNodeProxy<T>* parentItem;
        if(index.parent().isValid())
            parentItem = static_cast<prismTreeNodeProxy<T>*>(index.parent().internalPointer());
        else
            parentItem  = rootNode().get();
        beginRemoveRows(index.parent(),index.row(),index.row());
        //std::shared_ptr<prismTreeNodeProxy<T>> to_del_node = parentItem->child(index.row());
        //to_del_node.reset();
        parentItem->m_childItems.erase(parentItem->m_childItems.begin()+index.row());
        endRemoveRows();
        return true;
    }

    bool removeNodeAllchildren(QVariant to_del_node) override
    {
        QModelIndex index =  getIdexByData(to_del_node);
        prismTreeNodeProxy<T>* parentItem;
        if(index.isValid())
            parentItem = static_cast<prismTreeNodeProxy<T>*>(index.internalPointer());
        else
            parentItem  = rootNode().get();
        beginRemoveRows(index,0, rowCount(index)-1);
        parentItem->m_childItems.clear();
        endRemoveRows();
        return true;
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
