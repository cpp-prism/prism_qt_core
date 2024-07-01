#ifndef PRISM_QT_CORE_HPP_PRISMQT_JSON_HPP
#define PRISM_QT_CORE_HPP_PRISMQT_JSON_HPP
#include "prismQt.hpp"
#include "prismTreeModelProxy.hpp"
#include <QDateTime>
#include <QString>
#include <prism/prismJson.hpp>
#include <type_traits>
namespace prism::json::privates
{
// extend QString
template <class T>
struct jsonValue<T, std::enable_if_t<std::is_same_v<QString, T>,
                                     void>> : public jsonValueBase<jsonValue<T>>
{
    static void append_value(std::ostringstream& stream, [[maybe_unused]] const char* fname, T&& qstring, [[maybe_unused]] int identation, [[maybe_unused]] int&& level)
    {
        std::string  value = qstring.toStdString();

        if (!is_null_string(value))
        {
            stream << '"';
            for (const auto& ch : value)
            {
                if (ch == '"')
                {
                    stream << "\\\"";
                }
                else
                {
                    stream << ch;
                }
            }
            stream << '"';
        }
        else
        {
            stream << "null";
        }
    }

    static void from_jsonStr(T&& model, std::string_view&& str, int start, int end)
    {
        if (str[start] == '"')
        {
            bool isEscaped = false;
            std::string unquotedStr;
            for (int i = start; i < end; ++i)
            {
                auto& c = str[i];
                if (isEscaped)
                {
                    unquotedStr += c;
                    isEscaped = false;
                }
                else if (c == '\\')
                {
                    isEscaped = true;
                }
                else if (c != '"')
                {
                    unquotedStr += c;
                }
            }
            model = QString::fromStdString(unquotedStr);
        }
        else if (!(end - start == 3 &&
                   str[start] == 'n' &&
                   str[start + 1] == 'u' &&
                   str[start + 2] == 'l' &&
                   str[start + 3] == 'l'))
        {
            throw R"("c++ type is string, but json str not start with '"' or null")";
        }
    }
};

// extend QStringList
template <class T>
struct jsonArray<T, std::enable_if_t<std::is_same_v<QStringList, T>,
                                     void>> : public jsonArrayBase<jsonArray<T>>
{
    constexpr static void append_sub_values(std::ostringstream& stream, [[maybe_unused]] const char* fname, T&& value, int identation, int&& level)
    {
        auto a = value.begin();
        while (value.size() && a != value.end())
        {
            if (a != value.begin())
            {
                stream << ',';
            }
            if (identation)
            {
                if (a != value.begin())
                    stream << std::endl;
                for (int i = 0; i < identation * (level); ++i)
                    stream << " ";
            }
            using v_t = typename T::value_type;
            jsonType<v_t>::type::append_value(stream, nullptr, std::move(*a), identation, std::move(level));

            a++;
        }
    }

    constexpr static void read_sub_value(T&& model, std::string_view&& str, int start, int end)
    {
        using ft_ = typename T::value_type;
        model.push_back({});
        jsonType<ft_>::type::from_jsonStr(std::move(model.last()), std::move(str), start, end);
    }
};
// extend QList QVector
template <class T>
struct jsonArray<T, std::enable_if_t<prism::utilities::is_specialization<T, QVector>::value ||
                                         prism::utilities::is_specialization<T, QList>::value,
                                     void>> : public jsonArrayBase<jsonArray<T>>
{
    constexpr static void append_sub_values(std::ostringstream& stream, [[maybe_unused]] const char* fname, T&& value, int identation, int&& level)
    {
        auto a = value.begin();
        while (value.size() && a != value.end())
        {
            if (a != value.begin())
            {
                stream << ',';
            }
            if (identation)
            {
                if (a != value.begin())
                    stream << std::endl;
                for (int i = 0; i < identation * (level); ++i)
                    stream << " ";
            }
            using v_t = typename T::value_type;
            jsonType<v_t>::type::append_value(stream, nullptr, std::move(*a), identation, std::move(level));

            a++;
        }
    }

    constexpr static void read_sub_value(T&& model, std::string_view&& str, int start, int end)
    {
        using ft_ = typename T::value_type;
        model.append({});
        jsonType<ft_>::type::from_jsonStr(std::move(model.last()), std::move(str), start, end);
    }
};

// extend QDatetime
template <class T>
struct jsonValue<T, std::enable_if_t<std::is_same_v<T, QDateTime>,
                                     void>> : public jsonValueBase<jsonValue<T>>
{
    constexpr static void append_value(std::ostringstream& stream, [[maybe_unused]] const char* fname, T&& value, [[maybe_unused]] int identation, [[maybe_unused]] int&& level)
    {
        stream << '"';
        stream << value.toString("yyyy-MM-ddTHH:mm:ssZ").toStdString();
        stream << '"';
    }

    static void from_jsonStr(T&& model, std::string_view&& str, int start, int end)
    {
        if (str[start] == '"')
        {
            std::string unquotedStr;
            bool isEscaped = false;

            for (int i = start; i < end; ++i)
            {
                auto& c = str[i];
                if (isEscaped)
                {
                    unquotedStr += c;
                    isEscaped = false;
                }
                else if (c == '\\')
                {
                    isEscaped = true;
                }
                else if (c != '"')
                {
                    unquotedStr += c;
                }
            }
            model = QDateTime::fromString(QString::fromStdString(unquotedStr), "yyyy-MM-ddTHH:mm:ssZ");
        }
        else
            throw R"(the string of json value does not start with ")";
    }
};

// QMap QHash
template <class T>
struct jsonObject<T, std::enable_if_t<prism::utilities::is_specialization<T, QMap>::value ||
                                          prism::utilities::is_specialization<T, QHash>::value,
                                      void>> : public jsonObjectBase<jsonObject<T>>
{
    constexpr static void append_sub_kvs([[maybe_unused]] std::ostringstream& stream, [[maybe_unused]] const char* fname, [[maybe_unused]] T&& value, [[maybe_unused]] int identation, [[maybe_unused]] int&& level)
    {
        int count = 0;
        QKeyValueIterator it = value.keyValueBegin();
        while (it != value.keyValueEnd())
        {
            if (count)
                stream << ',';
            if (identation)
            {
                if (count)
                    stream << std::endl;
                for (int i = 0; i < identation * (level); ++i)
                    stream << " ";
            }
            jsonType<std::remove_cv_t<std::remove_reference_t<decltype(it->second)>>>::type::append_key_value(stream, it->first.toStdString().c_str(), std::move(it->second),
                                                                                                              identation, std::move(level));
            it++;
            ++count;
        }
    }
    static void read_sub_kv([[maybe_unused]] T&& model, [[maybe_unused]] std::string_view&& str, [[maybe_unused]] int kstart, [[maybe_unused]] int kend, [[maybe_unused]] int vstart, [[maybe_unused]] int vend)
    {
        QString s = QString::fromStdString(std::string(str.substr(kstart, kend - kstart)));
        using second_type = decltype(std::declval<decltype(model.keyValueBegin())>()->second);
        using ft_ = std::remove_cv_t<std::remove_reference_t<second_type>>;
        jsonType<ft_>::type::from_jsonStr(std::move(model[s]), std::move(str), vstart, vend);
    }
};

// prismTreeNodeProxy
template <class T>
struct jsonObject<T, std::enable_if_t<prism::utilities::is_specialization<T, prism::qt::core::prismTreeNodeProxy>::value,
                                      void>> : public jsonObjectBase<jsonObject<T>>
{
    constexpr static inline const char* childitems_name = "childItems";

    constexpr static void append_sub_kvs([[maybe_unused]] std::ostringstream& stream, [[maybe_unused]] const char* fname, [[maybe_unused]] T&& value, [[maybe_unused]] int identation, [[maybe_unused]] int&& level)
    {
        using valueType_t = std::remove_reference_t<std::remove_reference_t<decltype(*value.instance())>>;
        int count = 0;
        // if(!value.instance())
        //     value.m_instance = std::make_shared<valueType_t>();
        prism::reflection::for_each_fields<prism::utilities::const_hash("json")>(*value.instance(), [&](const char* ffname, auto&& value_) {
            if (count)
            {
                stream << ',';
            }
            if (identation)
            {
                if (count)
                    stream << std::endl;
                for (int i = 0; i < identation * (level); ++i)
                    stream << " ";
            }
            using v_t = std::remove_reference_t<std::remove_reference_t<decltype(value_)>>;
            jsonType<v_t>::type::append_key_value(stream, ffname, std::move(value_), identation, std::move(level));

            ++count;
        });

        //子节点
        stream << ',';
        stream << std::endl;
        for (int i = 0; i < identation * (level); ++i)
            stream << " ";
        jsonType<std::vector<std::shared_ptr<prism::qt::core::prismTreeNodeProxy<valueType_t>>>>::type::append_key_value(stream, childitems_name, std::move(value.m_childItems), identation, std::move(level));
    }

    static void read_sub_kv([[maybe_unused]] T&& model, [[maybe_unused]] std::string_view&& str, [[maybe_unused]] int kstart, [[maybe_unused]] int kend, [[maybe_unused]] int vstart, [[maybe_unused]] int vend)
    {
        using valueType_t = std::remove_reference_t<std::remove_reference_t<decltype(*model.instance())>>;
        auto s = std::string(str.substr(kstart, kend - kstart));
        // if(!model.instance())
        //     model.m_instance = std::make_shared<valueType_t>();
        if (s != std::string(childitems_name))
        {
            prism::reflection::field_do<prism::utilities::const_hash("json")>(*model.instance(), s.c_str(), [&](auto&& value) {
                using ft_ = std::remove_reference_t<std::remove_reference_t<decltype(value)>>;
                jsonType<ft_>::type::from_jsonStr(std::move(value), std::move(str), vstart, vend);
            });
        }
        else
        {
            jsonType<std::vector<std::shared_ptr<prism::qt::core::prismTreeNodeProxy<valueType_t>>>>::type::from_jsonStr(std::move(model.m_childItems), std::move(str), vstart, vend);
        }
    }
};
// prismTreeModelProxy
template <class T>
struct jsonObject<T, std::enable_if_t<prism::utilities::is_specialization<T, prism::qt::core::prismTreeModelProxy>::value,
                                      void>> : public jsonObjectBase<jsonObject<T>>
{
    static inline const char* rootNodeName = "rootNode";

    constexpr static void append_sub_kvs([[maybe_unused]] std::ostringstream& stream, [[maybe_unused]] const char* fname, [[maybe_unused]] T&& value, [[maybe_unused]] int identation, [[maybe_unused]] int&& level)
    {
        using valueType_t = std::remove_reference_t<std::remove_reference_t<decltype(*value.rootNode()->instance())>>;
        for (int i = 0; i < identation * (level); ++i)
            stream << " ";
        stream << " \"";
        stream << rootNodeName;
        stream << "\":";
        jsonType<prism::qt::core::prismTreeNodeProxy<valueType_t>>::type::append_key_value(stream, fname, std::move(*value.rootNode()), identation, std::move(level));
    }

    static void read_sub_kv([[maybe_unused]] T&& model, [[maybe_unused]] std::string_view&& str, [[maybe_unused]] int kstart, [[maybe_unused]] int kend, [[maybe_unused]] int vstart, [[maybe_unused]] int vend)
    {
        using valueType_t = std::remove_reference_t<std::remove_reference_t<decltype(*model.rootNode()->instance())>>;
        auto s = std::string(str.substr(kstart, kend - kstart));
        if (s == std::string(rootNodeName))
        {
            std::shared_ptr<prism::qt::core::prismTreeNodeProxy<valueType_t>> rootNode = std::make_shared<prism::qt::core::prismTreeNodeProxy<valueType_t>>();
            rootNode->setInstance(std::make_shared<valueType_t>());
            jsonType<prism::qt::core::prismTreeNodeProxy<valueType_t>>::type::from_jsonStr(std::move(*rootNode), std::move(str), vstart, vend);
            model.setRootNode(rootNode);
        }
    };
};

// prismModelProxy
template <class T>
struct jsonObject<T, std::enable_if_t<prism::utilities::is_specialization<T, prism::qt::core::prismModelProxy>::value,
                                      void>> : public jsonObjectBase<jsonObject<T>>
{

    using jsonObjectBase<jsonObject<T>>::alias_map_;
    constexpr static void append_sub_kvs([[maybe_unused]] std::ostringstream& stream, [[maybe_unused]] const char* fname, [[maybe_unused]] T&& value, [[maybe_unused]] int identation, [[maybe_unused]] int&& level)
    {
        using valueType_t = std::remove_reference_t<std::remove_reference_t<decltype(*value.instance())>>;
        jsonObject<valueType_t>::append_sub_kvs(stream, fname, std::move(*value.instance()), identation, std::move(level));
    }

    static void read_sub_kv([[maybe_unused]] T&& model, [[maybe_unused]] std::string_view&& str, [[maybe_unused]] int kstart, [[maybe_unused]] int kend, [[maybe_unused]] int vstart, [[maybe_unused]] int vend)
    {
        using valueType_t = std::remove_reference_t<std::remove_reference_t<decltype(*model.instance())>>;
        if (!model.instance())
            model.setInstance(std::make_shared<valueType_t>());
        jsonObject<valueType_t>::read_sub_kv(std::move(*model.instance()), std::move(str), kstart, kend, vstart, vend);
    }
};
// prismModelListProxy
template <class T>
struct jsonArray<T, std::enable_if_t<prism::utilities::is_specialization<T, prism::qt::core::prismModelListProxy>::value,
                                     void>> : public jsonArrayBase<jsonArray<T>>
{

    constexpr static void append_sub_values(std::ostringstream& stream, [[maybe_unused]] const char* fname, T&& value, int identation, int&& level)
    {

        auto a = value.list()->begin();
        while (value.list()->size() && a != value.list()->end())
        {
            if (a != value.list()->begin())
            {
                stream << ',';
            }
            if (identation)
            {
                if (a != value.list()->begin())
                    stream << std::endl;
                for (int i = 0; i < identation * (level); ++i)
                    stream << " ";
            }
            using v_t = typename T::value_type;
            jsonType<std::shared_ptr<prism::qt::core::prismModelProxy<v_t>>>::type::append_value(stream, nullptr, std::move(*a), identation, std::move(level));

            a++;
        }
    }

    constexpr static void read_sub_value(T&& model, std::string_view&& str, int start, int end)
    {
        using ft_ = typename T::value_type;
        std::shared_ptr<ft_> m = std::make_shared<ft_>();
        model.appendItemNotNotify(m);
        auto lm = model.list()->last();
        jsonType<prism::qt::core::prismModelProxy<ft_>>::type::from_jsonStr(*lm, std::move(str), start, end);
    }
};

} // namespace prism::json::privates

#endif // PRISMQT_JSON_HPP
