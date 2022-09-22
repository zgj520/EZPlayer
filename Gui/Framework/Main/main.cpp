#include "mainPrivate.h"
#include "Main/Application/CustomApplication.h"
#include <QIcon>
#include <QQmlApplicationEngine>
#include "Main/MainWindow/ViewModels/MainWindowViewModel.h"
#include "../../GUIKit/GUIKitInterface.h"
#include "Main\MainWindow\ViewModels\PlayerViewModel.h"
#include "Utils\RegisterModel.h"

int main(int argc, char* argv[]) {
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);

    CustomApplication app(argc, argv);
    app.setWindowIcon(QIcon(""));

    QQmlApplicationEngine* pQmlEngine = new QQmlApplicationEngine;
    app.setProperty("qmlEngine", quintptr(pQmlEngine));
    if (pQmlEngine != nullptr) {
        pQmlEngine->addImportPath(QStringLiteral("qrc:/"));
    }
    REGISTER_VIEWMODEL(PlayerWindow, "PlayerWindow");
    REGISTER_VIEWMODEL(PlayerViewModel, "PlayerViewModel");

    //全局初始化GUIKit
    GUIKit::initGUIKit(pQmlEngine);

    MainWindowViewModel::Instance().showWindow();



    int ret = app.exec();
    return ret;
}