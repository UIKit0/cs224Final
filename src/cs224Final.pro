#
#   CS 224
#   Author : ayidi (Arthur Yidi)
#
TARGET = cs224Final

QT += core gui opengl

TEMPLATE = app

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
}

# glm library
LIB = ../lib
GLM = $${LIB}/glm-0.9.5.1
DEFINES += GLM_FORCE_RADIANS

# resources
RES = ../res
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
    window/smokeworld.cpp

HEADERS += \
    window/camera.h \
    window/openglwindow.h \
    window/window.h \
    window/world.h \
    window/smokeworld.h

RESOURCES += \
    $${RES}/resources.qrc

# ASSIMP
INCLUDEPATH += /contrib/projects/assimp/3.0.1270/include
LIBS += -L/contrib/projects/assimp/3.0.1270/lib \
    -lassimp

#  hide object files and misc
OBJECTS_DIR = $${OUT_PWD}/.obj
MOC_DIR = $${OUT_PWD}/.moc
RCC_DIR = $${OUT_PWD}/.rcc
UI_DIR = $${OUT_PWD}/.ui

QMAKE_CXXFLAGS += -std=c++11
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3 -fno-strict-aliasing
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra
