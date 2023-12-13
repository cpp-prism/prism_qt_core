#ifndef PRISM_QT_CORE_HPP_PRISMTREENODE_HPP
#define PRISM_QT_CORE_HPP_PRISMTREENODE_HPP

#include "../prismQt_core_global.h"
#include "prismModelProxy.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <QObject>
#include <QModelIndex>


namespace prism::qt::core{
template<class T>
class prismTreeNodeProxy :public prismModelProxy<T>,public std::enable_shared_from_this<prismTreeNodeProxy<T>>
{
    std::weak_ptr<prismTreeNodeProxy<T>> m_parentItem ;

public:
    using value_type = T;
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
        if(m_childItems.size() > row && row>=0 )
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
}// namespace prism::qt::core
#endif // PRISM_QT_CORE_HPP_PRISMTREENODE_HPP
