#ifndef PRISM_QT_CORE_HELPER_CONDITION_VARAIABLE_GUARD_H
#define PRISM_QT_CORE_HELPER_CONDITION_VARAIABLE_GUARD_H

#include "../prismQt_core_global.h"
#include <QEventLoop>
#include <memory>
#include <condition_variable>

namespace prism::qt::core {
class PRISMQT_CORE_EXPORT condition_varaiable_guard
{
public:
    condition_varaiable_guard([[maybe_unused]]std::condition_variable* p_cv);

    ~condition_varaiable_guard();

private:
    std::condition_variable* mp_cv;
};

}// namespace prism::qt::core
#endif // PRISM_QT_CORE_HELPER_CONDITION_VARAIABLE_GUARD_H
