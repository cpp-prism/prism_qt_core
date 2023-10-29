#ifndef PRISM_QT_CORE_HELPER_QEVENTLOOPGUARD_H
#define PRISM_QT_CORE_HELPER_QEVENTLOOPGUARD_H

#include "../prismQt_core_global.h"
#include <QEventLoop>
#include <memory>

namespace prism::qt::core {
class PRISMQT_CORE_EXPORT QEventLoopGuard
{
public:
    QEventLoopGuard(QEventLoop* eventLoop);

    ~QEventLoopGuard();

    void release()
    {
        eventLoop_ = nullptr;
    }

private:
    QEventLoop* eventLoop_;
};

}// namespace prism::qt::core
#endif // PRISM_QT_CORE_HELPER_QEVENTLOOPGUARD_H
