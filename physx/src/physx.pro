#
#   CS 224
#   Author : ayidi (Arthur Yidi)
#
TARGET = physx

QT += core gui opengl

TEMPLATE = app

#message($${CONFIG})

# PHYSX build types:
# -CHECKED debug mode + instruments
# -PROFILE instruments
# -RELEASE

PHYSX = $$(PHYSX_PATH)

isEmpty(PHYSX) {
    # default sunlab path
    PHYSX = /contrib/projects/physx/3.3.0
}

INCLUDEPATH += $${PHYSX}/Include

macx:LIBS += -L$${PHYSX}/Lib/osx64
unix:!macx:LIBS += -L$${PHYSX}/Lib/linux64

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT

    # PHYSX library
    DEFINES += NDEBUG

    !clang:LIBS += -Wl,--start-group
    LIBS += -lPvdRuntime \
            -lSimulationController \
            -lSceneQuery \
            -lLowLevel \
            -lLowLevelCloth \
            -lPhysX3 \
            -lPhysX3Vehicle \
            -lPhysX3Cooking \
            -lPhysX3Extensions \
            -lPhysX3CharacterKinematic \
            -lPhysXProfileSDK \
            -lPhysXVisualDebuggerSDK \
            -lPxTask \
            -lPhysX3Common
    !clang:LIBS += -Wl,--end-group

} else {
    # PHYSX library
    DEFINES += _DEBUG \
        PHYSX_PROFILE_SDK \
        PX_DEBUG \
        PX_CHECKED \
        PX_SUPPORT_VISUAL_DEBUGGER

    !clang:LIBS += -Wl,--start-group
    LIBS += -lPvdRuntimeCHECKED \
        -lSimulationControllerCHECKED \
        -lSceneQueryCHECKED \
        -lLowLevelCHECKED \
        -lLowLevelClothCHECKED \
        -lPhysX3CHECKED \
        -lPhysX3VehicleCHECKED \
        -lPhysX3CookingCHECKED \
        -lPhysX3ExtensionsCHECKED \
        -lPhysX3CharacterKinematicCHECKED \
        -lPhysXProfileSDKCHECKED \
        -lPhysXVisualDebuggerSDKCHECKED \
        -lPxTaskCHECKED \
        -lPhysX3CommonCHECKED
    !clang:LIBS += -Wl,--end-group
}

# PHYSX dependencies
#LIBS += -lm \
#        -lpthread \
#        -lrt

# GLM library
LIB = ../../lib
GLM = $${LIB}/glm-0.9.5.1
DEFINES += GLM_FORCE_RADIANS

# resources
RES = ../../res
GUI = $${RES}/gui
TEXTURES = $${RES}/textures

INCLUDEPATH += window $${GLM}
DEPENDPATH += window

SOURCES += \
    window/main.cpp \
    window/camera.cpp \
    window/openglwindow.cpp \
    window/window.cpp \
    window/world.cpp \
    assets/obj.cpp


HEADERS += \
    window/camera.h \
    window/openglwindow.h \
    window/window.h \
    window/world.h \
    assets/obj.h

RESOURCES += \
    $${RES}/resources.qrc

#  hide object files and misc
OBJECTS_DIR = $${OUT_PWD}/.obj
MOC_DIR = $${OUT_PWD}/.moc
RCC_DIR = $${OUT_PWD}/.rcc
UI_DIR = $${OUT_PWD}/.ui

clang {
    QMAKE_LFLAGS += -std=c++11 -stdlib=libc++
    QMAKE_CXXFLAGS += -stdlib=libc++
}

macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -fno-strict-aliasing
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra
