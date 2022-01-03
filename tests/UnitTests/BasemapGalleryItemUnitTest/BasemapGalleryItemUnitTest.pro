#-------------------------------------------------
#  Copyright 2021 ESRI
#
#  All rights reserved under the copyright laws of the United States
#  and applicable international laws, treaties, and conventions.
#
#  You may freely redistribute and use this sample code, with or
#  without modification, provided you include the original copyright
#  notice and use restrictions.
#
#  See the Sample code usage restrictions document for further information.
#-------------------------------------------------
QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_basemapgalleryitemunittest.cpp

HEADERS += \
    tst_basemapgalleryitemunittest.h

QT += testlib
QT -= gui

ARCGIS_RUNTIME_VERSION = 100.14
macx {
    cache()
}

QT += core gui opengl xml testlib network positioning sensors multimedia quick

# Uncomment to run against the setup
CONFIG+=build_from_setup

DEFINES += UNIT_TESTS

# This block determines whether to build against the installed SDK or the local dev build area
CONFIG(build_from_setup) {
  message("building against the installed SDK")
  include($$PWD/../arcgisruntime.pri)
} else {
  include ($$PWD/../../../../DevBuildCpp.pri)
}

CONFIG(debug, debug|release){
    DEFINES += DEBUG
}

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

win32:CONFIG += embed_manifest_exe
win32:DEFINES += _CRT_SECURE_NO_WARNINGS

macx | ios {
  message($$QT_ARCH)
  message("Using mac-clang")
  CONFIG += no_objective_c
}

win32:DEFINES += MSWINDOWS \
                 NOMINMAX

include($$PWD/../../../uitools/toolkitcpp.pri)
include($$PWD/../shared.pri)

