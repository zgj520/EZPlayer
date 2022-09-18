#include "CustomApplication.h"

CustomApplication::CustomApplication(int& argc, char** argv, int appFlags):
    QApplication(argc, argv, appFlags){

}

bool CustomApplication::notify(QObject* obj, QEvent* ev) {
    if (ev->type() == QEvent::MouseButtonPress) {
        emit mousePressed();
    }
    else if (ev->type() == QEvent::MouseButtonRelease) {
        emit mouseRelease();
    }
    return QApplication::notify(obj, ev);
}