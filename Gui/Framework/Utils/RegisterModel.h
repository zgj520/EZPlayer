#pragma once
#include <QMetaType>
#include <qqml.h>

#ifndef REGISTER_VIEWMODEL
#define REGISTER_VIEWMODEL(__CLASS_NAME, __URI)                                           \
    qRegisterMetaType<__CLASS_NAME>(QString(#__CLASS_NAME).append("Ptr").toUtf8().data());\
    qmlRegisterType<__CLASS_NAME>(__URI, 1, 0, #__CLASS_NAME);
#endif // !REGISTER_VIEWMODEL
