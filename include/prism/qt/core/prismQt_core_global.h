#ifndef PRISM_QT_CORE_GLOBAL_H
#define PRISM_QT_CORE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(PRISM_QT_CORE_LIBRARY)
#define PRISMQT_CORE_EXPORT Q_DECL_EXPORT
#else
#define PRISMQT_CORE_EXPORT Q_DECL_IMPORT
#endif

#endif // PRISM_QT_CORE_GLOBAL_H
