#ifndef TEST_MODEL_H
#define TEST_MODEL_H
#include "include/prism/qt/core/hpp/prismQt.hpp"
#include <prism/prismSql.hpp>
#include <QDateTime>


struct testTable{
    testTable(){}
    testTable(int id):my_int(id){}
    bool my_bool = true;
    int my_int = 132;
    float my_float = 1.23f;
    double my_double = 2.340;
    std::string my_str = "this is a std string";
};
PRISM_FIELDS(testTable,my_bool,my_int,my_float,my_double,my_str);
PRISM_FIELD_ATTRIBUTE(prism::sql::attributes::Attr_sql_field_isPrimaryKey,&testTable::my_int,true);



struct t2 {
    int my_int = 1;
};
PRISM_FIELDS(t2,my_int);
PRISMQT_TYPE(t2);

struct t1{
    ~t1()
    {
        delete ptr_t2;
    }
    std::shared_ptr<t2> sptr_t2 = std::make_shared<t2>();
    t2* ptr_t2 = new t2{};
};
PRISM_FIELDS(t1,sptr_t2,ptr_t2);
PRISMQT_TYPE(t1);

std::time_t initTimestamp(int year, int month, int day, int hour = 0, int min = 0, int second = 0)
{
    std::tm timeInfo = {};
    timeInfo.tm_year = year - 1900; // 年份从 1900 年开始计数
    timeInfo.tm_mon = month - 1;    // 月份从 0 开始计数
    timeInfo.tm_mday = day;
    timeInfo.tm_hour = hour;
    timeInfo.tm_min = min;
    timeInfo.tm_sec = second;

    std::time_t timestamp = std::mktime(&timeInfo);
    return timestamp;
}
std::chrono::system_clock::time_point initDatetime(int year, int month, int day, int hour = 0, int min = 0, int second = 0)
{
    std::time_t timestamp = initTimestamp(year, month, day, hour, min, second);
    return std::chrono::system_clock::from_time_t(timestamp);
}

struct st_test
{
    int my_bool = false;
    int my_int = 1;
    QString my_qstring = "232  \" sdf中文adfad";
    QString my_qstring_nll;
    qlonglong my_qlonglong = 3323;
    QDateTime my_qdatetime = QDateTime(QDate(2023, 01, 02), QTime(13, 14, 15));
    std::chrono::system_clock::time_point my_time_point = initDatetime(2222, 12, 22, 12, 12, 12);
    QList<int> my_qlist{11, 22, 11, 22};
    std::list<int> my_std_list{11, 11, 22, 22};
    QVector<int> my_qvector{22, 33};
    QMap<QString, int> my_qmap_k_string{{"key1", 1}};
    QHash<QString, int> my_qhash_qstring{{"key2", 2}};
    std::vector<QString> stdVecQString{"qstring1", "qstring"};
    QStringList qslist = {"qstr1","qstr2"};
};

PRISMQT_TYPE(std::chrono::system_clock::time_point)
PRISMQT_TYPE(st_test)
PRISM_FIELDS(st_test, my_bool, my_int, my_qlonglong, my_qstring_nll, my_qstring, my_qlist, my_qdatetime, my_time_point, my_qmap_k_string, my_qhash_qstring, stdVecQString,qslist)

#endif // TEST_MODEL_H
