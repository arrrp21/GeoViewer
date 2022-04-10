QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CommonImageTransformer.cpp \
    GprData.cpp \
    ImageLabel.cpp \
    QImageWrapper.cpp \
    Span.cpp \
    main.cpp \
    mainwindow.cpp

LIBS += -L"$$PWD/CL" -lOpenCL

INCLUDEPATH += "$$PWD/CL" \
        "$$PWD/kernels"

HEADERS += \
    CommonImageTransformer.hpp \
    GprData.hpp \
    ImageLabel.hpp \
    ImageTransformer.hpp \
    QImageWrapper.hpp \
    Span.hpp \
    mainwindow.h \
    $$files(*.h, true) \
    $$PWD/CL/cl2.hpp \
    $$PWD/CL/opencl.hpp \
    $$files(*.cl)

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
