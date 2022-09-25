#ifndef PlayerViewModel_H_
#define PlayerViewModel_H_
#include <QWindow>
#include <windows.h>
#include <QtQuick/QQuickWindow>
#include "PlayerWindow.h"
#include <QtQuick/QQuickItem>

class PlayerViewModel : public QObject {
    Q_OBJECT

public:
    PlayerViewModel(QObject* parent = nullptr);

    Q_PROPERTY(PlayerWindow* playerRenderWindow READ playerRenderWindow);

    Q_PROPERTY(QWindow* playerLayerWindow READ playerLayerWindow);

    Q_INVOKABLE PlayerWindow* playerRenderWindow();

    Q_INVOKABLE QWindow* playerLayerWindow();

    Q_INVOKABLE void createPlayerRenderWindow(QQuickWindow* parent);

    Q_INVOKABLE void dropEventDeal(const QString &files);

    Q_INVOKABLE QString convertUS2String(int64_t currentTime);

    Q_INVOKABLE bool pause();

    Q_INVOKABLE bool resume();

    Q_INVOKABLE bool seekTime(int64_t time);

signals:
    void signalPlayProgressChanged(int64_t currentTime, int64_t totalTime);

    void signalPlayStateChanged(bool isPlaying);

private:
    void playCallBack(int64_t currentTime, int64_t totalTime);

    void playWindowDropCallBack(const std::string& file);

private:
    PlayerWindow* m_pPlayRenderWindow = nullptr;
    QQuickWindow* m_pPlayerLayerWindow = nullptr;
};

#endif