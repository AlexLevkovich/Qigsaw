# -------------------------------------------------
# Project created by QtCreator 2009-03-24T12:12:48
# -------------------------------------------------
TARGET = qigsaw
TEMPLATE = app

QT       += core gui multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

SOURCES += main.cpp \
    mainwindow.cpp \
    SquareCutter.cpp \
    RectCutter.cpp \
    QuadCutter.cpp \
    Piece.cpp \
    NoRotationsShuffler.cpp \
    MultiPiece.cpp \
    Knob.cpp \
    JigUtil.cpp \
    JigShuffler.cpp \
    JigsawCutter.cpp \
    IGap.cpp \
    FreeShuffler.cpp \
    FreeCenterShuffler.cpp \
    exceptions.cpp \
    ClassicCutter.cpp \
    Classic4Cutter.cpp \
    ByColorShuffler.cpp \
    newdialog.cpp \
    jigsawaboutbox.cpp \
    mainwidget.cpp \
    previewframe.cpp \
    finaldialog.cpp \
    preview.cpp \
    aqsound.cpp \
    qpreviewfiledialog.cpp
HEADERS += mainwindow.h \
    SquareCutter.h \
    RectCutter.h \
    QuadCutter.h \
    Piece.h \
    NoRotationsShuffler.h \
    MultiPiece.h \
    Knob.h \
    JigUtil.h \
    JigShuffler.h \
    JigsawCutter.h \
    IGap.h \
    FreeShuffler.h \
    FreeCenterShuffler.h \
    exceptions.h \
    ClassicCutter.h \
    Classic4Cutter.h \
    ByColorShuffler.h \
    newdialog.h \
    jigsawaboutbox.h \
    TDArray.h \
    mainwidget.h \
    previewframe.h \
    finaldialog.h \
    preview.h \
    aqsound.h \
    qpreviewfiledialog.h
FORMS += mainwindow.ui \
    NewDialog.ui \
    jigsawaboutbox.ui \
    mainwidget.ui \
    previewframe.ui \
    finaldialog.ui
RESOURCES += qigsaw.qrc
win32:RC_FILE += qigsaw.rc
linux-g++:LIBS += -lasound
TRANSLATIONS = qigsaw_ru.ts \
    qigsaw_be.ts

LUPDATE = $$[QT_INSTALL_BINS]/lupdate -locations relative -no-ui-lines -no-sort
LRELEASE = $$[QT_INSTALL_BINS]/lrelease

updatets.files = TRANSLATIONS
updatets.commands = $$LUPDATE $$PWD/qigsaw.pro

QMAKE_EXTRA_TARGETS += updatets

updateqm.depends = updatets
updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_BASE}.qm
updateqm.commands = $$LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_OUT}
updateqm.name = LRELEASE ${QMAKE_FILE_IN}
updateqm.variable_out = PRE_TARGETDEPS
updateqm.CONFIG += no_link
QMAKE_EXTRA_COMPILERS += updateqm
