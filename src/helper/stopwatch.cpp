#include "include/prism/qt/core/helper/stopwatch.h"
#include "include/prism/qt/core/hpp/prismQtJson.hpp"

PRISM_FIELDS(prism::qt::core::TraceEntry,name,cat,ph,pid,tid,ts,dur,args);

namespace prism::qt::core{

StopWatcher::StopWatcher(QString title)
{
    start = std::chrono::high_resolution_clock::now();
    e.name = title;
    e.ph = "X"; //一个执行过程，可以由两个条目 ph:b  ph:e  表示，也可以使用一个条目ph:X + dur 表示
    e.pid = 0;
    e.tid = (qlonglong)QThread::currentThreadId();
    e.ts = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - getStart()).count();
    e.args.insert("开始时间",QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz"));
    getEntries().append(e);
}

StopWatcher::~StopWatcher()
{
    e.dur = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start).count();
    e.args.insert("结束时间",QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss:zzz"));
    getEntries().append(e);
}

time_point_t StopWatcher::getStart()
{
    static const time_point_t start =std::chrono::high_resolution_clock::now();
    return start;
}

QList<TraceEntry> &StopWatcher::getEntries()
{
    static QList<TraceEntry> entries;
    return entries;
}

void StopWatcher::dump2file()
{
    QFile file(QString("dump.json"));
    file.remove();
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        stream << QString::fromStdString(prism::json::toJsonString(getEntries(),4));
    }
    qDebug()<< "traces had save:" << getEntries().size();
}


}// namespace prism::qt::core
