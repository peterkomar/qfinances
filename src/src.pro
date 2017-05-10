TEMPLATE = app
CONFIG += warn_on \
    thread \
    qt \
 debug #uncomment for debug
#release #uncomment for production

TARGET = ../finances

OBJECTS_DIR = ../build
MOC_DIR = ../build
UI_DIR = ../build
RCC_DIR = ../build

# Components
include(modules/account/account.pri)
include(modules/currency/currency.pri)
include(modules/category/category.pri)

SOURCES += main.cpp \
    finances.cpp \
    topheader.cpp \
    navpanel.cpp \
    modules.cpp \
    database.cpp \
    logindialog.cpp \
    grid.cpp \
    module.cpp \
    modulewidget.cpp \
    financesapp.cpp \
    dialog.cpp \
    model.cpp \
    navigationwidget.cpp \
    treecombobox.cpp \
    properties.cpp
HEADERS += finances.h \
    topheader.h \
    navpanel.h \
    modules.h \
    database.h \
    logindialog.h \
    grid.h \
    module.h \
    modulewidget.h \
    project.h \
    financesapp.h \
    dialog.h \
    model.h \
    navigationwidget.h \
    treecombobox.h \
    properties.h

RESOURCES += application.qrc
win32{
  RC_FILE = winicon.rc
}
macx: {
  ICON = pictures/myfinances2.icns
}
QT += widgets sql printsupport charts

FORMS +=

DISTFILES +=
