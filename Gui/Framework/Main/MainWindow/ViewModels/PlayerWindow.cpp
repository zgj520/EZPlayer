#include "PlayerWindow.h"
#include "../../Core/Interface/CoreInterface.h"
#include <QImage>
#include <QTimer>

PlayerWindow::PlayerWindow(QWindow* parent):QWindow(parent) {
    m_wnd = (HWND)this->winId();
    DragAcceptFiles(m_wnd, true);
}

bool PlayerWindow::nativeEvent(const QByteArray& eventType, void* message, qintptr* result) {
    auto msg = (MSG*)message;
    if (msg->message == WM_PAINT) {
        if (m_bOpenGLRender) {
            ::BeginPaint(m_wnd, nullptr);
            ::EndPaint(m_wnd, nullptr);
            if (result) {
                *result = 0;
            }
            return true;
        }
    }
    else if (msg->message == WM_DROPFILES) {
        auto s = parent();
        HDROP hdrop = (HDROP)msg->wParam;
        char sDropFilePath[MAX_PATH];
        int iDropFileNums = 0;
        iDropFileNums = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, NULL);
        if (iDropFileNums > 0) {
            DragQueryFile(hdrop, 0, sDropFilePath, sizeof(sDropFilePath));
            play(sDropFilePath);
        }
        DragFinish(hdrop);//释放文件名缓冲区
    }
    return QWindow::nativeEvent(eventType,message, result);
}

bool PlayerWindow::event(QEvent*ev) {
    if (ev->type() == QEvent::Show || ev->type() == QEvent::Resize) {
        refreshCurrentFrame();
    }
    return QWindow::event(ev);
}

void PlayerWindow::play(const std::string& filePath) {
    if (filePath.empty()) {
        return;
    }
    if (m_filePath != filePath) {
        if (m_renderInstance != 0) {
            EZCore::destoryFileInstance(m_renderInstance);
            m_renderInstance = 0;
        }
        m_filePath = filePath;
    }
    if (m_renderInstance == 0) {
        m_renderInstance = EZCore::createFileInstance(m_filePath, this->winId());
    }
    EZCore::play(m_renderInstance);
    m_bOpenGLRender = true;
}

void PlayerWindow::refreshCurrentFrame() {
    if (m_renderInstance != 0) {
        EZCore::refreshCurrentFrame(m_renderInstance);
    }
}