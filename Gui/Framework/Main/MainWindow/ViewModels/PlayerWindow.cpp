#include "PlayerWindow.h"
#include <QImage>
#include <QTimer>

PlayerWindow::PlayerWindow(QWindow* parent):QWindow(parent) {
    m_wnd = (HWND)this->winId();
    DragAcceptFiles(m_wnd, true);
}

void PlayerWindow::setDropCallBack(DROP_CALLBACK cbk) {
    m_dropCallBack = cbk;
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
        if (m_dropCallBack != nullptr) {
            HDROP hdrop = (HDROP)msg->wParam;
            char sDropFilePath[MAX_PATH];
            int iDropFileNums = 0;
            iDropFileNums = DragQueryFile(hdrop, 0xFFFFFFFF, NULL, NULL);
            if (iDropFileNums > 0) {
                DragQueryFile(hdrop, 0, sDropFilePath, sizeof(sDropFilePath));
                m_dropCallBack(sDropFilePath);
            }
            DragFinish(hdrop);
        }
    }
    return QWindow::nativeEvent(eventType,message, result);
}

bool PlayerWindow::event(QEvent*ev) {
    if (ev->type() == QEvent::Show || ev->type() == QEvent::Resize) {
        refreshCurrentFrame();
    }
    return QWindow::event(ev);
}

void PlayerWindow::play(const std::string& filePath, EZCore::PLAY_CALLBACK cbk) {
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
    EZCore::play(m_renderInstance, cbk);
    m_bOpenGLRender = true;
}

bool PlayerWindow::resume() {
    return EZCore::resume(m_renderInstance);
}

bool PlayerWindow::pause() {
    return EZCore::pause(m_renderInstance);
}

bool PlayerWindow::seekTime(int64_t time) {
    return EZCore::seekTime(m_renderInstance, time);
}

void PlayerWindow::refreshCurrentFrame() {
    if (m_renderInstance != 0) {
        EZCore::refreshCurrentFrame(m_renderInstance);
    }
}