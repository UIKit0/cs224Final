#
#   CS 224
#   Author : ayidi (Arthur Yidi)
#
TARGET = cs224Final

QT += core gui opengl

TEMPLATE = app

# qpainter doesn't support core profile
DEFINES += ENABLE_CORE_PROFILE

CONFIG(release, debug|release) {
    DEFINES += QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT
} else {
    DEFINES += DEBUG_OPENGL
}

# quick fix solution - TODO: change glsw to use res
SHADERS = $$(SHADER_PATH)

isEmpty(SHADERS) {
    SHADERS = ../res/shaders/
}
DEFINES += SHADER_PATH=\\\"$${SHADERS}\\\"

ODE = $$(ODE_PATH)

isEmpty(ODE) {
    # default sunlab path
    ODE = /contrib/projects/ODE/0.13
}

# ODE library
LIBS += -L$${ODE}/lib \
        -lode \
        -lm \
        -lpthread

INCLUDEPATH += $${ODE}/include

LIB = ../lib
RES = ../res

# glsw shader wrangler tool
GLSW = $${LIB}/glsw-2010
# TODO: compile to outside lib
SOURCES += \
    $${GLSW}/glsw/bstrlib.c \
    $${GLSW}/glsw/glsw.c \

# glm library
GLM = $${LIB}/glm-0.9.5.1
DEFINES += GLM_FORCE_RADIANS

INCLUDEPATH += . $${GLM} $${GLSW}
DEPENDPATH += .

SOURCES += \
    window/main.cpp \
    scene/camera.cpp \
    window/openglwindow.cpp \
    window/window.cpp \
    scene/world.cpp \
    assets/obj.cpp \
    graphics/program.cpp \
    particles/smokeparticle.cpp \
    particles/particleemitter.cpp \
    particles/basicsmokeemitter.cpp \
    particles/smoketrailemitter.cpp \
    interaction/windvolume.cpp \
    interaction/vortex.cpp \
    interaction/vortexshedder.cpp \
    interaction/solidobject.cpp \
    scene/matrixstack.cpp \
    scene/global.cpp \
    scene/terrain.cpp \
    particles/particles.cpp \
    scene/contour.cpp \
    scene/player.cpp \
    interaction/bullet.cpp \
    scene/terrainobject.cpp

HEADERS += \
    scene/camera.h \
    window/openglwindow.h \
    window/window.h \
    scene/world.h \
    assets/obj.h \
    graphics/debug.h \
    graphics/program.h \
    particles/smokeparticle.h \
    particles/particleemitter.h \
    particles/basicsmokeemitter.h \
    particles/smoketrailemitter.h \
    interaction/windvolume.h \
    interaction/vortex.h \
    interaction/vortexshedder.h \
    interaction/solidobject.h \
    scene/matrixstack.h \
    scene/global.h \
    scene/terrain.h \
    particles/particles.h \
    scene/player.h \
    interaction/collisions.h \
    interaction/bullet.h \
    scene/terrainobject.h \
    scene/contour.h

RESOURCES += \
    $${RES}/resources.qrc

OTHER_FILES += \
    $${RES}/shaders/contour.glsl \
    $${RES}/shaders/smoke.glsl \
    $${RES}/shaders/terrain.glsl

# install shaders (todo: replace wrangler loading from rcc)
extra.path += $${OUT_PWD}/shaders
extra.file += $${OTHER_FILES}
INSTALLS += extra

#  hide object files and misc
OBJECTS_DIR = $${OUT_PWD}/.obj
MOC_DIR = $${OUT_PWD}/.moc
RCC_DIR = $${OUT_PWD}/.rcc
UI_DIR = $${OUT_PWD}/.ui

clang {
    QMAKE_LFLAGS += -std=c++11 -stdlib=libc++
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7
}

macx:QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.7

gcc {
    QMAKE_CXXFLAGS += -std=c++11
    QMAKE_CXXFLAGS_RELEASE -= -O2
    QMAKE_CXXFLAGS_RELEASE += -O3 -fno-strict-aliasing
    QMAKE_CXXFLAGS_WARN_ON -= -Wall
    QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                              -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                              -Wsequence-point -Wsign-compare -Wstrict-aliasing -Wstrict-overflow=1 -Wswitch \
                              -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                              -Wvolatile-register-var -Wno-extra
}
