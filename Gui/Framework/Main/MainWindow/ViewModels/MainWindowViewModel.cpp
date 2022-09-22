#include "MainWindowViewModel.h"
#include "Utils/RegisterModel.h"
#include "../../Log/Interface/EZLogInterface.h"
#include "Utils/CreateQMLObj.h"
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
    m_pMainWindow = qobject_cast<QQuickWindow*>(createObjByQMLFile(m_qmlStr.toStdString()));
    if (m_pMainWindow == nullptr) {
        return;
    }
    m_pMainWindow->create();
    m_pMainWindow->show();
    m_pMainWindow->raise();
}

MainWindowViewModel::MainWindowViewModel(const QString& urlPath, QObject* parent)
    :QObject(parent){
    //REGISTER_VIEWMODEL(MainWindowViewModel, "MainWindow");
    m_qmlStr = urlPath;
}
MainWindowViewModel::~MainWindowViewModel() {

}