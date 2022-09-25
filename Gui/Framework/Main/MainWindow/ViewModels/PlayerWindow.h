#ifndef PlayerWindow_H_
#define PlayerWindow_H_
#include <QWindow>
#include <windows.h>
#include <QtQuick/QQuickWindow>
#include "../../Core/Interface/CoreInterface.h"

using DROP_CALLBACK = std::function<void(const std::string&)>;
class PlayerWindow : public QWindow {
    Q_OBJECT

public:
    PlayerWindow(QWindow* parent);

    ~PlayerWindow();

    void setDropCallBack(DROP_CALLBACK cbk);

    void play(const std::string& filePath, EZCore::PLAY_CALLBACK cbk);

    bool resume();

    bool pause();

    bool seekTime(int64_t time);

    void refreshCurrentFrame();

private:
    bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;

    bool event(QEvent*) override;

private:
    HWND m_wnd = nullptr;
    long m_renderInstance = 0;
    std::string m_filePath;
    bool m_bOpenGLRender = false;

    DROP_CALLBACK m_dropCallBack = nullptr;
};

#endif