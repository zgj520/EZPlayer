#include "PlayerViewModel.h"
#include "Utils/CreateQMLObj.h"

PlayerViewModel::PlayerViewModel(QObject* parent):QObject(parent) {
    
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
}

void PlayerViewModel::dropEventDeal(const QString& files){
    auto file = files.right(files.size() - 8).toStdString();
    m_pPlayRenderWindow->play(file);
    m_pPlayRenderWindow->show();
}