#ifndef PRISM_QT_CORE_HPP_PRISMQT_JSON_HPP
#define PRISM_QT_CORE_HPP_PRISMQT_JSON_HPP
#include "prismQt.hpp"
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
        jsonValue<std::string>::append_value(stream, fname, qstring.toStdString(), identation, std::move(level));
    }

    static void from_jsonStr(T&& model, std::string_view&& str, int start, int end)
    {
        std::string stdstr;
        jsonValue<std::string>::from_jsonStr(std::move(stdstr), std::move(str), start, end);
        model = QString::fromStdString(stdstr);
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

// extend QList QVector
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

}// namespace prism::json::privates

#endif // PRISMQT_JSON_HPP
