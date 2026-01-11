QT -= gui
QT += core network

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TEMPLATE = app
TARGET = ServerApp_tests

# include headers of ServerApp
INCLUDEPATH += ../ServerApp

SOURCES += \
    test_fileservice.cpp \
    test_main.cpp \
    test_protocol.cpp \
    ../ServerApp/FileService.cpp \
    ../ServerApp/CommandHandler.cpp \
    ../ServerApp/CommandDispatcher.cpp \
    ../ServerApp/ClientSession.cpp \
    ../ServerApp/Protocol.cpp

HEADERS += \
    ../ServerApp/FileService.h \
    ../ServerApp/CommandHandler.h \
    ../ServerApp/CommandDispatcher.h \
    ../ServerApp/ClientSession.h \
    ../ServerApp/Protocol.h
# Link with gtest
LIBS += -lgtest -pthread
