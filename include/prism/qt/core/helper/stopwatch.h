#ifndef PRISM_QT_CORE_STOPWATCHER_H
#define PRISM_QT_CORE_STOPWATCHER_H
#include <QDebug>
#include <chrono>
#include <QString>
#include <QStringList>
#include <QList>
#include <QThread>
#include <QDateTime>
#include <QFile>
#include "../prismQt_core_global.h"

#ifdef PERFORMANCE_TEST
#define STOPWATCHER(VAR,NAME)  prism::qt::core::StopWatcher VAR(#NAME);
#define STOPWATCHER_DUMP2FILE  prism::qt::core::StopWatcher::dump2file();
#else
#define STOPWATCHER(VAR,NAME)
#define STOPWATCHER_DUMP2FILE
#endif

//STOPWATCH(var,name)

namespace prism::qt::core{

#if defined(Q_OS_ANDROID) || defined(Q_OS_MAC) || defined(Q_OS_WIN)
using time_point_t = std::chrono::steady_clock::time_point;
#elif defined(Q_OS_LINUX)
using time_point_t = std::chrono::system_clock::time_point;
#else
// fallback
using time_point_t = std::chrono::steady_clock::time_point;
#endif

/**
 * @brief The traceEntry class
 * 使用原理可以参照：https://limboy.me/posts/chrome-trace-viewer/
 */
struct PRISMQT_CORE_EXPORT TraceEntry{
    QString name ;
    QString cat ;
    QString ph;
    qlonglong pid ;
    qlonglong tid;
    long ts;
    long dur=0;
    QMap<QString,QString> args;
};

struct PRISMQT_CORE_EXPORT StopWatcher
{
    StopWatcher(QString title);

    ~StopWatcher();

private:
    TraceEntry e;
    time_point_t start;

    static time_point_t getStart();
    static QList<TraceEntry>& getEntries();
public:
    static void dump2file();
};

}//  namespace prism::qt::core

#endif // PRISM_QT_CORE_STOPWATCHER_H

