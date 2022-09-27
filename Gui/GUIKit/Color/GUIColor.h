#ifndef GUICLOLOR_H_
#define GUICLOLOR_H_

#include <QColor>
#include <QObject>
#include <QQmlEngine>

#ifndef GUI_COLOR_DEFINE
#define GUI_COLOR_DEFINE(__COLOR_NAME, __COLOR)                 \
public:                                                         \
    Q_PROPERTY(QColor __COLOR_NAME READ __COLOR_NAME CONSTANT)  \
    QColor __COLOR_NAME() {                                    \
        QColor __COLOR_NAME##Color(__COLOR);             \
        return __COLOR_NAME##Color;                             \
    }
#endif // !GUI_COLOR_DEFINE


class GUIColor : public QObject {
    Q_OBJECT

public:
    static GUIColor* Instance() {
        static GUIColor* g_color = new GUIColor;
        return g_color;
    }
    GUI_COLOR_DEFINE(Clear, "transparent");
    GUI_COLOR_DEFINE(Cyan0, "#213539");
    GUI_COLOR_DEFINE(Cyan1, "#004D52");
    GUI_COLOR_DEFINE(Cyan2, "#00C1CD");
    GUI_COLOR_DEFINE(Black00, "#000000");
    GUI_COLOR_DEFINE(Black0, "#070709");
    GUI_COLOR_DEFINE(Black0B, "#121212");
    GUI_COLOR_DEFINE(Black1, "#1B1B1C");
    GUI_COLOR_DEFINE(Black2, "#202023");
    GUI_COLOR_DEFINE(Black3, "#252528");
    GUI_COLOR_DEFINE(Black4, "#29292D");
    GUI_COLOR_DEFINE(Black5, "#343337");
    GUI_COLOR_DEFINE(Gray0, "#4c4c4c");
    GUI_COLOR_DEFINE(Gray1, "#6c6c6c");
    GUI_COLOR_DEFINE(Gray2, "#808080");
    GUI_COLOR_DEFINE(Gray3, "#999999");
    GUI_COLOR_DEFINE(Gray4, "#b2b2b2");
    GUI_COLOR_DEFINE(Gray5, "#d4d4d4");
    GUI_COLOR_DEFINE(Gray6, "#e5e5e5");
    GUI_COLOR_DEFINE(Red0, "#450b0b");
    GUI_COLOR_DEFINE(Red1, "#ae1c1c");
    GUI_COLOR_DEFINE(Yellow0, "#d4a500");
    GUI_COLOR_DEFINE(White00, "#ffffff");
    GUI_COLOR_DEFINE(White0, "#fafafa");
    GUI_COLOR_DEFINE(White08, "#fafafa08");

protected:
    GUIColor() = default;
    GUIColor(GUIColor&) = delete;
    GUIColor(GUIColor&&) = delete;
};
#endif