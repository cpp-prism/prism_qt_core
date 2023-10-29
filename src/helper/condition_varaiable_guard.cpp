#include "include/prism/qt/core/helper/condition_varaiable_guard.h"

namespace prism::qt::core {

condition_varaiable_guard::condition_varaiable_guard(std::condition_variable* p_cv):mp_cv(p_cv)
{

}

condition_varaiable_guard::~condition_varaiable_guard()
{
    if(mp_cv)
        mp_cv->notify_one();
}


}// namespace prism::qt::core
