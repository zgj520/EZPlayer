#ifndef GUIKITINTERFACE_H_
#define GUIKITINTERFACE_H_

#include <qglobal.h>
#include <QQmlEngine>

#if defined(GUIKIT_EXPORT_API)
#define GUIKIT_EXPORT Q_DECL_EXPORT
#else 
#define GUIKIT_EXPORT Q_DECL_IMPORT
#endif

namespace GUIKit {
    void GUIKIT_EXPORT initGUIKit(QQmlEngine* engine);
}

#endif