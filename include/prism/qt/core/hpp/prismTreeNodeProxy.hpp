#ifndef PRISM_QT_CORE_HPP_PRISMTREENODE_HPP
#define PRISM_QT_CORE_HPP_PRISMTREENODE_HPP

#include "../prismQt_core_global.h"
#include "prismModelProxy.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <QObject>
#include <QModelIndex>
#include <stdexcept>


namespace prism::qt::core{

template<class T>
class prismTreeNodeProxy :public prismModelProxy<T>,public std::enable_shared_from_this<prismTreeNodeProxy<T>>
{
    std::weak_ptr<prismTreeNodeProxy<T>> m_parentItem ;

public:
    using value_type = T;

    static void recurseNodeDo(prismTreeNodeProxy<T>* node ,std::function<void(prismTreeNodeProxy<T>*)> lambda)
    {
        for(std::shared_ptr<prismTreeNodeProxy<T>> item : node->childItems())
        {
            recurseNodeDo(item.get(),lambda);
        }
        lambda(node);
    }
    static bool recurseNodePredict(prismTreeNodeProxy<T>* node ,std::function<bool(prismTreeNodeProxy<T>*)> lambda)
    {
        bool result = false;
        for(std::shared_ptr<prismTreeNodeProxy<T>> item : node->childItems())
        {
            result |= recurseNodePredict(item.get(),lambda);
            if(result)
                return result;
        }
        return result |= lambda(node);
    }

    std::vector<std::shared_ptr<prismTreeNodeProxy<T>>> m_childItems ;
    std::vector<std::shared_ptr<prismTreeNodeProxy<T>>>  childItems(){
        return m_childItems;
    }
    std::shared_ptr<T> instance() const
    {
        return  prismModelProxy<T>::m_instance;
    }
    void setInstance(std::shared_ptr<T> p)
    {
        prismModelProxy<T>::m_instance = p;
    }
    explicit prismTreeNodeProxy<T>(std::shared_ptr<T> instance = std::make_shared<T>()):prismModelProxy<T>(nullptr,instance)
    {
        QQmlEngine::setObjectOwnership(this,QQmlEngine::ObjectOwnership::CppOwnership);
    }

    void setParentItem(std::shared_ptr<prismTreeNodeProxy<T>> parent)
    {
        m_parentItem = parent;
    }
    std::shared_ptr<prismTreeNodeProxy<T>> parentItem() const
    {
        if(auto sp =m_parentItem.lock())
        {
            return  sp;
        }
        else
            return nullptr;
    }

    void appendChild(std::shared_ptr<prismTreeNodeProxy<T>> child){
        child->m_parentItem = this->shared_from_this();
        QQmlEngine::setObjectOwnership(child.get(),QQmlEngine::ObjectOwnership::CppOwnership);
        m_childItems.push_back(child);
    }
    void removeChild(std::shared_ptr<prismTreeNodeProxy<T>> child)
    {
        auto it =std::find(m_childItems.begin(),m_childItems.end(),child);
        if(it != m_childItems.end())
            m_childItems.erase(it);
    }



    std::shared_ptr<prismTreeNodeProxy<T>> child(int row)
    {
        int size = static_cast<int>(m_childItems.size());
        if(size > row && row>=0 )
            return m_childItems.at(row);
        else
            return nullptr;
    }
    int childCount() const
    {
        return  static_cast<int>(m_childItems.size());
    }
    int row() const
    {
        if(parentItem())
        {
            auto it = std::find_if(parentItem()->m_childItems.begin(),parentItem()->m_childItems.end(),[=](std::shared_ptr<prismTreeNodeProxy<T>> node){
                return  instance().get() ==   node->instance().get();
            });
            if(it != parentItem()->m_childItems.end())
            {
                return std::distance(parentItem()->m_childItems.begin(), it);
            }
        }

        return 0;
    }

};

template<class T>
struct prismTreeNodeProxyRef{
    prismTreeNodeProxyRef(std::shared_ptr<prismTreeNodeProxy<T>> data,std::string keyPath):data_(data),keyPath_(keyPath) {}
    T* operator ->()
    {
        return data_->instance().get();
    }
    bool empty()
    {
        return data_ == nullptr;
    }
    std::shared_ptr<prismTreeNodeProxy<T>> data(){
        return data_;
    }
    const std::string keyPath() const
    {
        return keyPath_;
    }
    prismTreeNodeProxyRef<T> operator[](std::string key)
    {
        if(!data_)
            return prismTreeNodeProxyRef<T>(nullptr,keyPath_);

        for(std::shared_ptr<prismTreeNodeProxy<T>> item: data_->childItems())
        {
			QVariant qv = item->get(QByteArray::fromStdString(keyPath_));
            std::string getstr = qv.value<QString>().toStdString();
			if (getstr == key)
				return prismTreeNodeProxyRef<T>(item, keyPath_);
        }
        return prismTreeNodeProxyRef<T>(nullptr,keyPath_);
    }
private:
    std::shared_ptr<prismTreeNodeProxy<T>> data_;
    std::string keyPath_;
};

}// namespace prism::qt::core
#endif // PRISM_QT_CORE_HPP_PRISMTREENODE_HPP
