#ifndef PRISM_QT_CORE_PRISMDATAACCESSLAYER_HPP
#define PRISM_QT_CORE_PRISMDATAACCESSLAYER_HPP

#include <prism/prismSql.hpp>
#include "../helper/prismdataaccesslayerbase.h"

namespace prism::qt::core {
template<class func>
struct Sql_logic:public Sql_logic_base
{
    func f_;
    Sql_logic(func&& f):f_(std::forward<func>(f))
    {
    }
    void run(std::shared_ptr<QSqlDatabase> db) override
    {
        f_(db);
    }
};



template<class T>
class prismDataAccessLayer :public prismDataAccessLayerBase
{
    using db_type_t = T;
    using sql_type_t = ::prism::sql::Sql<T> ;
private:
    constexpr const char* getQtDatabaseType()
    {
        if constexpr (std::is_same_v<db_type_t, ::prism::sql::sqlite3::Sqlite3>)
        {
            return "QSQLITE";
        }
        else
        return "";
    }
public:
    prismDataAccessLayer(QString cachName,QString database="",QString ip="",QString username="",QString pwd="" )
        :prismDataAccessLayerBase(QString::fromStdString(std::string(getQtDatabaseType())),cachName,database,ip,username,pwd)
    {
    }

    template<class T>
    bool deleteTable()
    {
        bool result = this->run(Sql_logic([](std::shared_ptr<QSqlDatabase> db)
                                {
                                    std::string sql_create_table = sql_type_t::deleteTable<T>();
                                    QSqlQuery query(*db);
                                    QString sql = QString::fromStdString(sql_create_table);
                                    qDebug() << "================================================================ ";
                                    qDebug() << " exec delete table  [thread:" << quintptr(QThread::currentThreadId()) << "]";
                                    qDebug() << "================================================================ ";
                                    qDebug().noquote() << sql;
                                    if(query.exec(sql))
                                    {
                                        qDebug()<< "sql exec success";
                                    }
                                    else
                                    {
                                        qDebug()<< "sql exec error :" << query.lastError();
                                    }
                                }));
        return result;
    }
    template<class T>
    bool createTable()
    {
        bool result = this->run(Sql_logic([](std::shared_ptr<QSqlDatabase> db)
                                {
                                    std::string sql_create_table = sql_type_t::createTable<T>();
                                    QSqlQuery query(*db);
                                    QString sql = QString::fromStdString(sql_create_table);
                                    qDebug() << "================================================================ ";
                                    qDebug() << " exec create table  [thread:" << quintptr(QThread::currentThreadId()) << "]";
                                    qDebug() << "================================================================ ";
                                    qDebug().noquote() << sql;
                                    if(query.exec(sql))
                                    {
                                        qDebug()<< "sql exec success";
                                    }
                                    else
                                    {
                                        qDebug()<< "sql exec error :" << query.lastError();
                                    }
                                }));
        return result;
    }

    template<class T>
    bool insert(std::vector<std::shared_ptr<T>> models)
    {
        bool result = this->run(Sql_logic([&](std::shared_ptr<QSqlDatabase> db)
                                {
                                    std::string sql_create_table = sql_type_t::insert<T>(models);
                                    QSqlQuery query(*db);
                                    QString sql = QString::fromStdString(sql_create_table);
                                    qDebug() << "================================================================ ";
                                    qDebug() << " exec insert into table  [thread:" << quintptr(QThread::currentThreadId()) << "]";
                                    qDebug() << "================================================================ ";
                                    qDebug().noquote() << sql;
                                    if(query.exec(sql))
                                    {
                                        qDebug()<< "sql exec success";
                                    }
                                    else
                                    {
                                        qDebug()<< "sql exec error :" << query.lastError();
                                    }
                                }));
        return result;
    }
    template<class T>
    std::shared_ptr<std::vector<std::shared_ptr<T>>> queryTable(const char* where = nullptr)
    {
        std::shared_ptr<std::vector<std::shared_ptr<T>>> result = std::make_shared<std::vector<std::shared_ptr<T>>>();
        this->run(Sql_logic([&](std::shared_ptr<QSqlDatabase> db)
                  {

                      std::string sql_create_table = sql_type_t::queryTable<T>(where);
                      QSqlQuery query(*db);
                      QString sql = QString::fromStdString(sql_create_table);
                      qDebug() << "================================================================ ";
                      qDebug() << " exec query table  [thread:" << quintptr(QThread::currentThreadId()) << "]";
                      qDebug() << "================================================================ ";
                      qDebug().noquote() << sql;
                      if(query.exec(sql))
                      {

                          int size =0;
                          if(query.last())
                          {
                              size =  query.at() + 1;
                              query.first();
                              query.previous();
                          }
                          result->reserve(size);
                          qDebug()<< "sql exec success, size:" << size;
                          //if(query.size()>=0)// sqlite not support .size(),but mysql
                          if(size>0)
                          {
                              while(query.next())
                              {
                                  std::shared_ptr<T>  row = std::make_shared<T>();
                                  int i = 0;
                                  ::prism::reflection::for_each_fields(*row,[&](const char* fname,auto&& value)
                                  {
                                      std::shared_ptr<T>  row = std::make_shared<T>();
                                      std::optional<bool> ignore = ::prism::attributes::getFieldAttr<T,::prism::sql::attributes::Attr_sql_field_ignore>(fname);
                                      std::optional<const char* > datatype = ::prism::attributes::getFieldAttr<T,::prism::sql::sqlite3::attributes::Attr_sql_field_datatype>(fname);
                                      using v_t = std::decay_t<std::decay_t<decltype(value)>>;

                                      if(ignore.has_value() && ignore)
                                      return;

                                      if(!std::strcmp(datatype.value() , "TEXT"))
                                      {
                                          if constexpr (std::is_same_v<std::string,v_t>)
                                          {
                                              value = query.value(i).value<QString>().toStdString();
                                          }
                                      }
                                      else
                                      {
                                          std::stringstream ss;
                                          ss << query.value(i).toString().toStdString();
                                          ss >> value;
                                      }
                                      ++i ;
                                  });
                                  result->push_back(row);
                              }
                          }
                      }
                      else
                      {
                          qDebug()<< "sql exec error :" << query.lastError();
                      }
                  }));
        return result;
    }
};

} //namespace prism::qt::core

#endif // PRISM_QT_CORE_PRISMDATAACCESSLAYER_HPP
