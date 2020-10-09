QT += quick
CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
HEADERS += \
    Src/TaoItem.h \
    Src/TaoRenderer.h \
    Src/TaoDecoder.h

SOURCES += \
        Src/main.cpp \
    Src/TaoItem.cpp \
    Src/TaoRenderer.cpp \
    Src/TaoDecoder.cpp

RESOURCES += \
    Qml.qrc \
    Shaders.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =
FFmpegPath = $$_PRO_FILE_PWD_/FFmpeg



win32 {
    contains(QT_ARCH, x86_64) {
        #x64
        LIBS += -L$$FFmpegPath/lib/win64
        BinPath = $$_PRO_FILE_PWD_/bin/win64
        message("win64")
    } else {
        # 32
        LIBS += -L$$FFmpegPath/lib/win32
        BinPath = $$_PRO_FILE_PWD_/bin/win32
        message("win32")
    }
}
macos {
    LIBS += -L$$FFmpegPath/lib/macos
    BinPath = $$_PRO_FILE_PWD_/bin/macos
    message("macos")
}

INCLUDEPATH += $$FFmpegPath/include
LIBS += -lavutil -lavcodec -lavformat

DESTDIR = $$BinPath

OTHER_FILES += \
    README.md \
    LICENSE \
    .github/workflows/*


