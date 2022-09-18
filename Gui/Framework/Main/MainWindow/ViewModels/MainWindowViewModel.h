#ifndef MAINWINDOWVIEWMODEL_H_
#define MAINWINDOWVIEWMODEL_H_

#include <QQmlApplicationEngine>
#include <QtQuick/QQuickWindow>

class QQuickItem;
class MainWindowViewModel : public QObject {
    Q_OBJECT

public:
    static MainWindowViewModel& Instance();
    void showWindow(QWindow* parent = nullptr);

    Q_INVOKABLE QQuickWindow* getPlayerWindow() { return m_pMainWindow; };

protected:
    explicit MainWindowViewModel(const QString& urlPath, QObject* parent = nullptr);
    virtual ~MainWindowViewModel();

private:
    QQuickWindow* m_pMainWindow = nullptr;
    QString m_qmlStr;
};











#endif