#include "GUIKitInterface.h"
#include "Color/GUIColor.h"
#include "Font/GUIFont.h"

namespace GUIKit {
    void initGUIKit(QQmlEngine* engine) {
        if (engine == nullptr) {
            return;
        }
        engine->addImportPath(QStringLiteral("qrc:/"));
        qmlRegisterSingletonType<GUIColor>("GUIKit", 1, 0, "GUIColor", [](QQmlEngine* engine, QJSEngine* scriptEngine) {return GUIColor::Instance(); });
        qmlRegisterSingletonType<GUIFont>("GUIKit", 1, 0, "GUIFont", [](QQmlEngine* engine, QJSEngine* scriptEngine) {return GUIFont::Instance(); });
    }
}
