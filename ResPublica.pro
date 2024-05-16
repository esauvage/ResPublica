QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    dlgeditvote.cpp \
    dlglistevote.cpp \
    main.cpp \
    mainrespublica.cpp \
    rplistwidget.cpp \
    vote.cpp \
    voteGraphicItem.cpp \
    voteScene.cpp \
    voteliste.cpp

HEADERS += \
    VoteScene.h \
    dlgeditvote.h \
    dlglistevote.h \
    mainrespublica.h \
    rplistwidget.h \
    vote.h \
    voteGraphicItem.h \
    voteliste.h

FORMS += \
    dlgeditvote.ui \
    dlglistevote.ui \
    mainrespublica.ui

TRANSLATIONS += \
    ResPublica_fr_FR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
