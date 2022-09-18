#ifndef CUSTOMAPPLICATION_H_
#define CUSTOMAPPLICATION_H_

#include <QApplication>

class CustomApplication : public QApplication {
    Q_OBJECT

public:
    CustomApplication(int& argc, char** argv, int appFlags = ApplicationFlags);

signals:
    void mouseRelease();
    void mousePressed();

protected:
    bool notify(QObject* obj, QEvent* ev) override;
};
#endif // !CUSTOMAPPLICATION_H_
