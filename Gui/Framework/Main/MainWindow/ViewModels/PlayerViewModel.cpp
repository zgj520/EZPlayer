#include "PlayerViewModel.h"
#include "Utils/CreateQMLObj.h"

#define PLAY_CALL_BACK_FUNC std::bind(&PlayerViewModel::playCallBack, this, std::placeholders::_1, std::placeholders::_2)
PlayerViewModel::PlayerViewModel(QObject* parent):QObject(parent) {
    qRegisterMetaType<int64_t>("int64_t"); 
}

PlayerViewModel::~PlayerViewModel() {
    if (m_pPlayRenderWindow) {
        delete m_pPlayRenderWindow;
        m_pPlayRenderWindow = nullptr;
    }
}

PlayerWindow* PlayerViewModel::playerRenderWindow() {
    return m_pPlayRenderWindow;
}

QWindow* PlayerViewModel::playerLayerWindow() {
    return m_pPlayerLayerWindow;
}

void PlayerViewModel::createPlayerRenderWindow(QQuickWindow* parent) {
    if (m_pPlayRenderWindow != nullptr) {
        return;
    }
    m_pPlayRenderWindow = new PlayerWindow(parent);
    m_pPlayRenderWindow->setDropCallBack(std::bind(&PlayerViewModel::playWindowDropCallBack, this, std::placeholders::_1));
}

void PlayerViewModel::dropEventDeal(const QString& files){
    auto file = files.right(files.size() - 8).toStdString();
    EZCore::MediaInfo info;
    EZCore::getMediaInfo(file, info);
    emit signalResolutionChanged(info.width, info.height);
    m_pPlayRenderWindow->play(file, PLAY_CALL_BACK_FUNC);
    m_pPlayRenderWindow->show();
    emit signalPlayStateChanged(true);
}

QString PlayerViewModel::convertUS2String(int64_t currentTime) {
    int64_t totalSecends = currentTime / 1000 / 1000;
    int64_t hour = totalSecends / 60 / 60;
    int64_t mins = totalSecends / 60 % 60;
    int64_t secs = totalSecends % 60;
    return QString("%1:%2:%3").arg(hour, 2, 10, QLatin1Char('0')).arg(mins, 2, 10, QLatin1Char('0')).arg(secs, 2, 10, QLatin1Char('0'));
}

bool PlayerViewModel::pause() {
    if (m_pPlayRenderWindow) {
        return m_pPlayRenderWindow->pause();
    }
    return false;
}

bool PlayerViewModel::resume() {
    bool ret = false;
    if (m_pPlayRenderWindow) {
        return m_pPlayRenderWindow->resume();
    }
    return false;
}

bool PlayerViewModel::seekTime(int64_t time) {
    if (!m_pPlayRenderWindow) {
        return false;
    }
    m_pPlayRenderWindow->seekTime(time);
}

void PlayerViewModel::playCallBack(int64_t currentTime, int64_t totalTime) {
    emit signalPlayProgressChanged(currentTime, totalTime);
    if (currentTime >= totalTime) {
        emit signalPlayStateChanged(false);
    }
}

void PlayerViewModel::playWindowDropCallBack(const std::string& file) {
    if (m_pPlayRenderWindow) {
        EZCore::MediaInfo info;
        EZCore::getMediaInfo(file, info);
        emit signalResolutionChanged(info.width, info.height);
        m_pPlayRenderWindow->play(file, PLAY_CALL_BACK_FUNC);
        m_pPlayRenderWindow->show();
        emit signalPlayStateChanged(true);
    }
}
