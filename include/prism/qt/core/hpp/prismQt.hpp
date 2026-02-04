#ifndef PRISM_QT_CORE_HPP_PRISMQT_HPP
#define PRISM_QT_CORE_HPP_PRISMQT_HPP

#include "prismEnumProxy.hpp"
#include "prismModelListProxy.hpp"
#include "prismTreeModelProxy.hpp"
#include <prism/prism.hpp>
#include <QHash>
#include <QList>
#include <QVariant>

Q_DECLARE_METATYPE(std::string*)
Q_DECLARE_METATYPE(std::shared_ptr<std::string>)
Q_DECLARE_METATYPE(const char*)
Q_DECLARE_METATYPE(QList<std::string>)
Q_DECLARE_METATYPE(std::list<std::string>)
Q_DECLARE_METATYPE(std::vector<std::string>)
Q_DECLARE_METATYPE(std::vector<std::string>*)
Q_DECLARE_METATYPE(std::shared_ptr<std::vector<std::string>>)
Q_DECLARE_METATYPE(bool*)
Q_DECLARE_METATYPE(std::shared_ptr<bool>)
Q_DECLARE_METATYPE(std::vector<bool>)
Q_DECLARE_METATYPE(std::vector<bool>*)
Q_DECLARE_METATYPE(std::shared_ptr<std::vector<bool>>)
Q_DECLARE_METATYPE(float*)
Q_DECLARE_METATYPE(std::shared_ptr<float>)
Q_DECLARE_METATYPE(std::stdoptional<float>)
Q_DECLARE_METATYPE(std::vector<float>*)
Q_DECLARE_METATYPE(std::shared_ptr<std::vector<float>>)
Q_DECLARE_METATYPE(double*)
Q_DECLARE_METATYPE(std::shared_ptr<double>)
Q_DECLARE_METATYPE(std::stdoptional<double>)
Q_DECLARE_METATYPE(std::vector<double>*)
Q_DECLARE_METATYPE(std::shared_ptr<std::vector<double>>)
Q_DECLARE_METATYPE(long*)
Q_DECLARE_METATYPE(std::shared_ptr<long>)
Q_DECLARE_METATYPE(std::stdoptional<long>)
Q_DECLARE_METATYPE(std::stdoptional<ulong>)
Q_DECLARE_METATYPE(std::stdoptional<qlonglong>)
Q_DECLARE_METATYPE(std::stdoptional<qulonglong>)
Q_DECLARE_METATYPE(std::stdoptional<int8_t>)
Q_DECLARE_METATYPE(std::stdoptional<int16_t>)
Q_DECLARE_METATYPE(int*)
Q_DECLARE_METATYPE(std::shared_ptr<int>)
Q_DECLARE_METATYPE(std::vector<int>*)
Q_DECLARE_METATYPE(std::shared_ptr<std::vector<int>>)
//Q_DECLARE_METATYPE(long*)
Q_DECLARE_METATYPE(std::stdoptional<int32_t>)
Q_DECLARE_METATYPE(std::stdoptional<uint>)
Q_DECLARE_METATYPE(std::stdoptional<uint8_t>)
Q_DECLARE_METATYPE(std::stdoptional<uint16_t>)
Q_DECLARE_METATYPE(prism::qt::core::prismEnumProxyBase*)
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismEnumProxyBase>)



#define PRISMQT_ENUM(Class)                     \
Q_DECLARE_METATYPE(Class)                   \
Q_DECLARE_METATYPE(Class*)                  \
Q_DECLARE_METATYPE(std::stdoptional<Class>) \
Q_DECLARE_METATYPE(std::shared_ptr<Class>)  \
Q_DECLARE_METATYPE(std::list<Class>)        \
Q_DECLARE_METATYPE(std::vector<Class>)      \
Q_DECLARE_METATYPE(QVector<Class>)\
Q_DECLARE_METATYPE(prism::qt::core::prismEnumProxy<Class>*)                 \
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismEnumProxy<Class>>)


#define PRISMQT_CLASS(Class)                     \
Q_DECLARE_METATYPE(Class)                   \
Q_DECLARE_METATYPE(Class*)                  \
Q_DECLARE_METATYPE(std::stdoptional<Class>) \
Q_DECLARE_METATYPE(std::shared_ptr<Class>)  \
Q_DECLARE_METATYPE(std::list<Class>)        \
Q_DECLARE_METATYPE(std::vector<Class>)      \
Q_DECLARE_METATYPE(QVector<Class>)\
Q_DECLARE_METATYPE(prism::qt::core::prismModelProxy<Class>*)                 \
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismModelProxy<Class>>) \
Q_DECLARE_METATYPE(prism::qt::core::prismModelListProxy<Class>*)             \
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismModelListProxy<Class>>)\
Q_DECLARE_METATYPE(prism::qt::core::prismTreeNodeProxy<Class>*)             \
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismTreeNodeProxy<Class>>)\
Q_DECLARE_METATYPE(prism::qt::core::prismTreeModelProxy<Class>*)                 \
Q_DECLARE_METATYPE(std::shared_ptr<prism::qt::core::prismTreeModelProxy<Class>>)


#define PRISMQT_MODEL(Class)                \
Q_DECLARE_METATYPE(Class)                   \
Q_DECLARE_METATYPE(std::shared_ptr<Class>)  \
Q_DECLARE_METATYPE(Class*)                  \


#endif // PRISM_QT_CORE_HPP_PRISMQT_HPP
