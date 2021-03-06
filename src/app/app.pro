#-------------------------------------------------
#
# Project created by QtCreator 2018-07-10T20:35:44
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PictureCrypt
TEMPLATE = app
CONFIG -= console
RC_ICONS = icons/unlocked.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    main.cpp \
    controllerpc.cpp \
    view/viewpc.cpp \
    view/encryptdialog.cpp \
    view/aboutpc.cpp \
    model/modelpc.cpp \
    model/qaesencryption.cpp

HEADERS += \
    controllerpc.h \
    view/viewpc.h \
    view/encryptdialog.h \
    view/aboutpc.h \
    model/modelpc.h \
    model/qaesencryption.h

FORMS += \
    view/viewpc.ui \
    view/encryptdialog.ui \
    view/aboutpc.ui

RESOURCES += \
    resource.qrc \
    translations.qrc \
    fonts.qrc

CONFIG(debug, debug|release) {
    DESTDIR         = $$OUT_PWD/build/Debug
}else {
    DESTDIR         = $$OUT_PWD/build/Release
}
#win32:QMAKE_POST_LINK += windeployqt $$DESTDIR $$escape_expand(\\n)

TRANSLATIONS +=  \
    translations/picturecrypt_ru.ts \
    translations/picturecrypt_de.ts

# Some random stuff that helps CI
QMAKE_CXXFLAGS += -std=c++0x
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x040900
