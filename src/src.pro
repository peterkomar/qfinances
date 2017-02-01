TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
 release
TARGET = bin/qfinances
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build
SOURCES += main.cpp \
    finances.cpp \
    dockwidg.cpp \
    fdatabase.cpp \
    misc.cpp \
    topheader.cpp \
    mystatusbar.cpp \
    filterdlgimpl.cpp \
    helpbrowser.cpp \
    titlebar.cpp \
    mortgagordlgimpl.cpp \
    mytransactionslist.cpp \
    parametersdlgimpl.cpp \
    transactiondlgimpl.cpp \
    accountpanelimpl.cpp \
    preferencesdlgImpl.cpp \
    querytable.cpp
HEADERS += finances.h \
    dockwidg.h \
    fdatabase.h \
    misc.h \
    topheader.h \
    mystatusbar.h \
    filterdlgimpl.h \
    helpbrowser.h \
    titlebar.h \
    mortgagordlgimpl.h \
    mytransactionslist.h \
    parametersdlgimpl.h \
    transactiondlgimpl.h \
    preferencesdlgImpl.h \
    querytable.h \
    accountpanelimpl.h
RESOURCES += application.qrc
QT += widgets sql printsupport
TRANSLATIONS += lang/Ukrainian.ts \
 lang/German.ts \
 lang/Russian.ts

win32{
  RC_FILE = winicon.rc
}

INSTALL_DIR = $$system( echo $HOME/QFinances )
target.path = $$INSTALL_DIR
documentation.files = doc/*
translation.files = lang/*.qm
macx: {
  documentation.path = $$INSTALL_DIR/qfinances.app/Contents/Resources/doc
  translation.path = $$INSTALL_DIR/qfinances.app/Contents/Resources/lang
}
else: {
  documentation.path = $$INSTALL_DIR/doc
  translation.path = $$INSTALL_DIR/lang
}

INSTALLS += target \
    documentation \
    translation

FORMS += \
    parameters.ui \
    preferences.ui \
    transaction.ui \
    account.ui \
    mortgagordialog.ui \
    mortgagors.ui
