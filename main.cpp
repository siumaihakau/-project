#include "mainwindow.h"
#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/ayame_icon.png"));

    MainWindow window;
    window.show();
    return app.exec();
}