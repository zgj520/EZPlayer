#include "MainWindowViewModel.h"
#include "Utils/RegisterModel.h"
#include "../../Log/Interface/EZLogInterface.h"
#include <QApplication>
#include <QQmlComponent>
#include "PlayerWindow.h"

MainWindowViewModel& MainWindowViewModel::Instance() {
    static MainWindowViewModel viewModel = MainWindowViewModel("qrc:/Main/MainWindow/Views/MainWindow.qml", nullptr);
    return viewModel;
}

void MainWindowViewModel::showWindow(QWindow* parent) {
    if (m_pMainWindow != nullptr) {
        m_pMainWindow->show();
        m_pMainWindow->raise();
        return;
    }
    QQmlApplicationEngine* pEngine = (QQmlApplicationEngine*)(qApp->property("qmlEngine").value<quintptr>());
    if (pEngine == nullptr) {
        EZLOGE("MainWindowViewModel error, qmlEngine empty");
        return;
    }
    QQmlComponent component(pEngine, m_qmlStr);
    QObject* pObj = component.create();
    if (pObj == nullptr || (m_pMainWindow = qobject_cast<QQuickWindow*>(pObj)) == nullptr) {
        auto errorString = component.errorString();
        EZLOGE("create component error as %s", errorString.toStdString().c_str());
        return;
    }
    m_pMainWindow = qobject_cast<QQuickWindow*>(pObj);
    m_pMainWindow->create();
    PlayerWindow* player1 = new PlayerWindow(m_pMainWindow);
    m_pMainWindow->setProperty("playerWindow1", QVariant::fromValue(player1));

    PlayerWindow* player2 = new PlayerWindow(m_pMainWindow);
    m_pMainWindow->setProperty("playerWindow2", QVariant::fromValue(player2));
    PlayerWindow* player3 = new PlayerWindow(m_pMainWindow);
    m_pMainWindow->setProperty("playerWindow3", QVariant::fromValue(player3));
    PlayerWindow* player4 = new PlayerWindow(m_pMainWindow);
    m_pMainWindow->setProperty("playerWindow4", QVariant::fromValue(player4));
    m_pMainWindow->show();
    m_pMainWindow->raise();
    player1->play("F:\\Material\\1080p.mp4");
    player2->play("F:\\Material\\监狱医生\\监狱医生02.mp4");
    player3->play("F:\\Material\\监狱医生\\监狱医生03.mp4");
    player4->play("F:\\Material\\监狱医生\\监狱医生04.mp4");
}

MainWindowViewModel::MainWindowViewModel(const QString& urlPath, QObject* parent)
    :QObject(parent){
    //REGISTER_VIEWMODEL(MainWindowViewModel, "MainWindow");
    m_qmlStr = urlPath;
}
MainWindowViewModel::~MainWindowViewModel() {

}