#pragma once
#include <QObject>
#include <string>

QObject* createObjByQMLFile(const std::string& qmlFile);