#-------------------------------------------------
#
# Project created by QtCreator 2014-10-28T17:58:05
#
#-------------------------------------------------

QT       += core gui  network

TARGET = dm8148_gui
TEMPLATE = app
DEFINES *= HAVE_CONFIG_H   #qrencode 二维码库需添加

SOURCES += main.cpp\
        mainwindow.cpp \
    qrcode.cpp

HEADERS  += mainwindow.h \
    socket_protocol.h \
    qrcode.h

FORMS    += mainwindow.ui

RESOURCES += \
    resource/res.qrc

OTHER_FILES += \
    style.qss

linux-arm-gnueabi-g++{
#加入这两句，把工程构建目标修改成install，就可以自动安装到nfs
target.path=/root/nfs_rootfs/rfs_ti814x_park/opt
INSTALLS += target

}else{
    #LIBS +=  ../dvr_gui/mcfw_lib/libnew_lib.a
}
