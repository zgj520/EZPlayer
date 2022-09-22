#ifndef PlayerWindow_H_
#define PlayerWindow_H_
#include <QWindow>
#include <windows.h>
#include <QtQuick/QQuickWindow>

class PlayerWindow : public QWindow {
    Q_OBJECT

public:
    PlayerWindow(QWindow* parent);

    void play(const std::string& filePath);

    void refreshCurrentFrame();

private:
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

    bool event(QEvent*) override;

private:
    HWND m_wnd = nullptr;
    long m_renderInstance = 0;
    std::string m_filePath;
    bool m_bOpenGLRender = false;
};

#endif