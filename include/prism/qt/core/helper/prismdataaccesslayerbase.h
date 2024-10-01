#ifndef PRISMDATAACCESSLAYERBASE_H
#define PRISMDATAACCESSLAYERBASE_H

#include <memory>
#include <QThread>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QEventLoop>
#include "../prismQt_core_global.h"

namespace prism::qt::core {

struct PRISMQT_CORE_EXPORT Sql_logic_base
{
    ~Sql_logic_base(){}
    virtual void run([[maybe_unused]]std::shared_ptr<QSqlDatabase> db);
};



class PRISMQT_CORE_EXPORT prismDataAccessLayerBase:public QObject
{
    void init();
    std::shared_ptr<QSqlDatabase> getDb(QString dbtype,QString cachName,QString ip="",QString username="",QString pwd="", QString database="");
    bool validDb(std::shared_ptr<QSqlDatabase> db);


    void clearup();
    Q_OBJECT
    std::unique_ptr<QThread> thread_;
    QString dbtype_;
    QString cacheName_;
    QString ip_;
    QString userName_;
    QString pwd_;
    QString database_;

private slots:
    void run_logic([[maybe_unused]]Sql_logic_base* sql_logic,QEventLoop* el,bool* result);

public:
    explicit prismDataAccessLayerBase(QString dbtype,QString cachName,QString database="",QString ip="",QString username="",QString pwd="" );
    ~prismDataAccessLayerBase();
    bool run(Sql_logic_base&& sql_logic);

};
} //namespace prism::qt::core

#endif // PRISMDATAACCESSLAYERBASE_H
