#include "include/prism/qt/core/helper/qeventloopguard.h"


namespace prism::qt::core {
QEventLoopGuard::QEventLoopGuard(QEventLoop *eventLoop) : eventLoop_(eventLoop) {
    while(!eventLoop->isRunning());
}

QEventLoopGuard::~QEventLoopGuard()
{
    if (eventLoop_ && eventLoop_->isRunning())
    {
        eventLoop_->exit();
    }
}

}// namespace prism::qt::core
