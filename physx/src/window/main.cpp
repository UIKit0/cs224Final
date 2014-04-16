//#include "window.h"
#include <QtGui/QGuiApplication>
#include <QDebug>

#include <PxPhysicsAPI.h>
using namespace physx;

PxDefaultAllocator		gAllocator;
PxDefaultErrorCallback	gErrorCallback;

int main(int argc, char *argv[])
{
    qWarning("RUNNING ON DEBUG MODE!");

//    PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

    QGuiApplication app(argc, argv);

//    Window window;
//    window.resize(640, 480);
//    window.setTitle("cs224 Final");

//    window.show();
//    // will tick according to sreen refresh screen rate
//    window.setAnimating(true);

    return app.exec();
}

