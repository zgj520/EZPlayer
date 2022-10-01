#ifndef PlayerViewModel_H_
#define PlayerViewModel_H_
#include <QWindow>
#include <windows.h>
#include <QtQuick/QQuickWindow>
#include "PlayerWindow.h"
#include <QtQuick/QQuickItem>
#include "Utils\DefineQMLList.h"

class MediaInfoKeyValue: public QObject{
    Q_OBJECT
    Q_PROPERTY(QString key READ getKey);
    Q_PROPERTY(QVariant value READ getValue);
public:
    QString getKey()const { return strKey; }
    QVariant getValue() const { return vValue; }
    MediaInfoKeyValue(QString key, QVariant value) {
        strKey = key;
        vValue = value;
    }
    MediaInfoKeyValue(const MediaInfoKeyValue& a){
        strKey = a.getKey();
        vValue = a.getValue();
    }
    MediaInfoKeyValue& operator=(const MediaInfoKeyValue& ob)
    {
        strKey = ob.getKey();
        vValue = ob.getValue();
        return *this;
    }
private:
    QString strKey;
    QVariant vValue;
};

class PlayerViewModel : public QObject {
    Q_OBJECT

public:
    PlayerViewModel(QObject* parent = nullptr);

    ~PlayerViewModel();

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

    DEFINE_OBJECT_QMLLIST(PlayerViewModel, m_mediaoInfoKeyValuelist, MediaInfoKeyValue);

signals:
    void signalPlayProgressChanged(int64_t currentTime, int64_t totalTime);

    void signalResolutionChanged(int rw, int rh);

    void signalPlayStateChanged(bool isPlaying);

private:
    void playCallBack(int64_t currentTime, int64_t totalTime);

    void playWindowDropCallBack(const std::string& file);
    
    void updateMediaInfo(const std::string& file);
private:
    PlayerWindow* m_pPlayRenderWindow = nullptr;
    QQuickWindow* m_pPlayerLayerWindow = nullptr;
    EZCore::MediaInfo m_mediaInfo;
    QList<MediaInfoKeyValue> m_mediaoInfoKeyValuelist;
};

#endif