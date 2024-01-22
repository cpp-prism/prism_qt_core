#include "../../include/prism/qt/core/helper/prismdataaccesslayerbase.h"
#include "../../include/prism/qt/core/helper/condition_varaiable_guard.h"
#include <QDebug>
#include <QObject>
#include <thread>
#include <type_traits>
#include <utility>
#include <iostream>

namespace prism::qt::core {

void prism::qt::core::Sql_logic_base::run(std::shared_ptr<QSqlDatabase> db) { }

void prism::qt::core::prismDataAccessLayerBase::init() {
    thread_.reset(new QThread());
    QObject::moveToThread(thread_.get());
    thread_->start();
}

std::shared_ptr<QSqlDatabase> prism::qt::core::prismDataAccessLayerBase::getDb(QString dbtype, QString cachName, QString ip, QString username, QString pwd, QString database)
{
    std::shared_ptr<QSqlDatabase> db = std::make_shared<QSqlDatabase>();
    if (QSqlDatabase::connectionNames().contains(cachName))
    {
        db = std::make_shared<QSqlDatabase>(QSqlDatabase::database(cachName));

    }
    else
    {
        db = std::make_shared<QSqlDatabase>(QSqlDatabase::addDatabase(dbtype,cachName));
        if(dbtype!= "QSQLITE")
        {
            db->setHostName(ip);
            db->setUserName(username);
            db->setPassword(pwd);
            db->setDatabaseName(database);
        }
        else
            db->setDatabaseName(database);
    }
    if (!db->open()) {
        qCritical()<< "connect data base error :" << db->lastError().text();
    }

    return db;
}

bool prism::qt::core::prismDataAccessLayerBase::validDb(std::shared_ptr<QSqlDatabase> db)
{
    if(!db)
        return false;
    if(!db->isOpen())
    {
        qCritical() << "database is open:" << db->isOpen();
        return false;
    }
    if(db->isOpenError())
    {
        qCritical() << "database is open error:" <<db->lastError().text();
        return false;
    }
    if(!db->isValid())
    {
        qCritical() << "database is valid:" <<db->isValid();
        return false;
    }
    return true;
}

void prism::qt::core::prismDataAccessLayerBase::clearup() {
    if(thread_) {
        thread_->quit();
        thread_->wait();
    }
}

void prism::qt::core::prismDataAccessLayerBase::run_logic(Sql_logic_base *sql_logic, std::condition_variable *el, bool *result) {
    ::prism::qt::core::condition_varaiable_guard raii(el);
    auto db = getDb(dbtype_,cacheName_,ip_,userName_,pwd_,database_);
    if(validDb(db))
    {
        sql_logic->run(db);
    }
    else
    {
        *result = false;
    }
}

prism::qt::core::prismDataAccessLayerBase::prismDataAccessLayerBase(QString dbtype, QString cachName, QString database, QString ip, QString username, QString pwd):
    dbtype_(dbtype),
    cacheName_(cachName),
    ip_(ip),
    userName_(username),
    pwd_(pwd),
    database_(database)
{
    init();
}

prism::qt::core::prismDataAccessLayerBase::~prismDataAccessLayerBase() { clearup(); }

bool prism::qt::core::prismDataAccessLayerBase::run(Sql_logic_base &&sql_logic)
{
    std::mutex mux;
    std::unique_lock<std::mutex> lk(mux);
    std::condition_variable cv;

    bool result = true;

    this->metaObject()->invokeMethod( this
                                      ,"run_logic"
                                      ,Qt::QueuedConnection
                                      ,Q_ARG(Sql_logic_base*,&sql_logic)
                                      ,Q_ARG(std::condition_variable*,&cv)
                                      ,Q_ARG(bool*,&result)
                                      );

    cv.wait(lk);
    return result;
}

} //namespace prism::qt::core
