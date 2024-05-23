QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

DEFINES += _WIN32_WINNT

SOURCES += \
    cipher.cpp \
    dlgconnexion.cpp \
    dlgeditvote.cpp \
    dlglistevote.cpp \
    dlgresultats.cpp \
    main.cpp \
    mainrespublica.cpp \
    personne.cpp \
    question.cpp \
    questionliste.cpp \
    rplistwidget.cpp \
    vote.cpp \
    voteGraphicItem.cpp \
    voteScene.cpp

HEADERS += \
    VoteScene.h \
    VoteScene.h \
    cipher.h \
    dlgconnexion.h \
    dlgeditvote.h \
    dlglistevote.h \
    dlgresultats.h \
    mainrespublica.h \
    personne.h \
    question.h \
    questionliste.h \
    rplistwidget.h \
    vote.h \
    voteGraphicItem.h

FORMS += \
    dlgconnexion.ui \
    dlgeditvote.ui \
    dlglistevote.ui \
    dlgresultats.ui \
    mainrespublica.ui

TRANSLATIONS += \
    ResPublica_fr_FR.ts
CONFIG += lrelease
CONFIG += embed_translations

INCLUDEPATH += D:\Qt\Tools\mingw1120_64\opt\include

LIBS +=  \
    "D:\Qt\Tools\OpenSSLv3\Win_x64\lib\libcrypto.lib"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
