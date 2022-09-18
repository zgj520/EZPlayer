#ifndef GUIFONT_H_
#define GUIFONT_H_

#include <QFont>
#include <QObject>
#include <QFontMetrics>

#ifndef GUI_FONT_DEFINE
#define GUI_FONT_DEFINE(__FONT_NAME, __FONT_SIZE, _FONT_WEIGHT)                 \
public:                                                         \
    Q_PROPERTY(QFont __FONT_NAME READ __FONT_NAME CONSTANT)  \
    QFont __FONT_NAME() {                                    \
        QFont __FONT_NAME##Font;                             \
        __FONT_NAME##Font.setPixelSize(__FONT_SIZE);         \
        __FONT_NAME##Font.setWeight(_FONT_WEIGHT);           \
        return __FONT_NAME##Font;                            \
    }
#endif // !GUI_FONT_DEFINE

#ifdef Q_OS_WIN32
#define GUI_FONT_WEIGHT_400 QFont::Normal
#define GUI_FONT_WEIGHT_600 QFont::Bold
#else
#define GUI_FONT_WEIGHT_400 QFont::Normal
#define GUI_FONT_WEIGHT_600 QFont::DemiBold
#endif

class GUIFont : public QObject {
    Q_OBJECT

public:
    static GUIFont* Instance() {
        static GUIFont* g_font = new GUIFont;
        return g_font;
    }
    Q_INVOKABLE QRect getTextRect(const QFont& font, const QString& text) {
        QFontMetrics fontMetrics(font);
        return fontMetrics.boundingRect(text);
    }
    GUI_FONT_DEFINE(H1Head48,     48, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(H2Headline36, 36, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(H3Headline24, 24, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(H4Headline20, 20, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(H5Headline16, 16, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(H6Headline14, 14, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(Bold13, 13, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(SubtitleSemibold12, 12, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(SubtitleRegular12, 12, GUI_FONT_WEIGHT_400);
    GUI_FONT_DEFINE(ButtonSemibold11, 11, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(ButtonRegular11, 11, GUI_FONT_WEIGHT_600);
    GUI_FONT_DEFINE(OverlineRegular10, 10, GUI_FONT_WEIGHT_400);

protected:
    GUIFont() = default;
    GUIFont(GUIFont&) = delete;
    GUIFont(GUIFont&&) = delete;
};
#endif