QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ChooseMaskDialog.cpp \
    CommonImageTransformer.cpp \
    GprData.cpp \
    GpuImageTransformer.cpp \
    ImageData.cpp \
    ImageLabel.cpp \
    ImageTransformer.cpp \
    OpenCLErrors.cpp \
    Panel.cpp \
    QImageWrapper.cpp \
    StateMachine.cpp \
    Timer.cpp \
    main.cpp \
    mainwindow.cpp \
    format.cc

# LIBS += -L"$$PWD/CL" -lOpenCL
# LIBS += -L"C:/Windows/System32" -lOpenCL
LIBS += C:\Windows\System32\OpenCL.dll

INCLUDEPATH += "$$PWD/CL" \
        "$$PWD/kernels" \
        "C:/Windows/System32"

HEADERS += \
    ChooseMaskDialog.hpp \
    CircularBuffer.hpp \
    CommonImageTransformer.hpp \
    GprData.hpp \
    GpuImageTransformer.hpp \
    ImageData.hpp \
    ImageLabel.hpp \
    ImageTransformer.hpp \
    Log.hpp \
    Mask.hpp \
    OpenCLErrors.hpp \
    OpenCLUtils.hpp \
    Operation.hpp \
    Panel.hpp \
    PredefinedMasks.hpp \
    QImageWrapper.hpp \
    State.hpp \
    StateMachine.hpp \
    Timer.hpp \
    Visitor.hpp \
    fmt/core.h \
    fmt/format.h \
    fmt/format-inl.h \
    mainwindow.h \
    $$files(*.h, true) \
    $$PWD/CL/cl2.hpp \
    $$PWD/CL/opencl.hpp \
    $$files(*.cl)

FORMS += \
    ChooseMaskDialog.ui \
    Panel.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    kernels/GpuImageTransformer.cl \
    kernels/test.cl \
    kernels/test2.cl
