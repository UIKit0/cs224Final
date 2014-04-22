#include "window.h"
#include <QtGui/QGuiApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    qWarning("RUNNING ON DEBUG MODE!");


    QGuiApplication app(argc, argv);

    Window window;
    window.resize(960, 540);
    window.setTitle("cs224 Final");

    window.showFullScreen();
    // will tick according to sreen refresh screen rate
    window.setAnimating(true);

    return app.exec();
}

