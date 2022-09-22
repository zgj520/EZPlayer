#pragma once
#include "CreateQMLObj.h"
#include <QQmlApplicationEngine>
#include <QApplication>
#include <QQmlComponent>
#include "../../Log/Interface/EZLogInterface.h"

QObject* createObjByQMLFile(const std::string& qmlFile) {
    QQmlApplicationEngine* pEngine = (QQmlApplicationEngine*)(qApp->property("qmlEngine").value<quintptr>());
    if (pEngine == nullptr) {
        EZLOGE("createObjByQMLFile error, qmlEngine empty");
        return nullptr;
    }
    QQmlComponent component(pEngine, qmlFile.c_str());
    QObject* pObj = component.create();
    if (pObj == nullptr) {
        auto errorString = component.errorString();
        EZLOGE("create component error as %s", errorString.toStdString().c_str());
        return nullptr;
    }
    return pObj;
}