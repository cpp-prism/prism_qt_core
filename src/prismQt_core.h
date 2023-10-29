#ifndef PRISMQT_CORE_H
#define PRISMQT_CORE_H

#include "include/prism/qt/core/prismQt_core_global.h"
#include <prism/qt/modular/interfaces/intf_module.h>
#include <QObject>

namespace prism::qt::core{

class PRISMQT_CORE_EXPORT prismQt_core : public ::prism::qt::modular::intfModule
{
    Q_OBJECT
	Q_PLUGIN_METADATA(IID "::prism::qt::modular::intfModule/1.0" FILE "prismQt_core.json")
    Q_INTERFACES(prism::qt::modular::intfModule)

  public:
    prismQt_core();

    // intfModule interface
  public:
    bool register_types() override;
    bool init() override;
    bool install() override;
    bool uninstall() override;
    bool uninit() override;
};

}// namespace prism::qt::core
#endif // PRISMQT_CORE_H
