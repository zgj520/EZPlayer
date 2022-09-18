#pragma once
#include <qglobal.h>

#if defined(FRAMEWORK_EXPORT_API)
#define FRAMEWORK_EXPORT Q_DECL_EXPORT
#else
#define FRAMEWORK_EXPORT Q_DECL_IMPORT
#endif

extern "C" {
    int FRAMEWORK_EXPORT main(int argc, char* argv[]);
}
