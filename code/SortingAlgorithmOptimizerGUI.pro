QT += core gui widgets

TARGET = SortingAlgorithmOptimizerGUI
TEMPLATE = app

CONFIG += c++11

SOURCES += Cui_Zeyu_DSC2409006_CST207_Project_Group_202509_GUI.cpp

CONFIG -= debug_and_release debug_and_release_target

DESTDIR = .

QMAKE_CXXFLAGS += -std=c++11

win32 {
    CONFIG += console
}

DEFINES += QT_DEPRECATED_WARNINGS
