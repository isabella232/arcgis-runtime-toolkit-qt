QT += testlib
QT -= gui

TEMPLATE = app

SOURCES +=  tst_searchviewfunctest.cpp

HEADERS += \
    tst_searchviewfunctest.h

ARCGIS_RUNTIME_VERSION = 100.13
macx {
    cache()
}

QT += core gui opengl xml testlib network positioning sensors multimedia quick

# Uncomment to run against the setup
CONFIG+=build_from_setup

DEFINES += UNIT_TESTS
#PCH_HEADER = $${PWD}/pch.hpp

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

ios {
    include (iOS/iOS.pri)
}

win32:DEFINES += MSWINDOWS \
                 NOMINMAX

# below tools include is not used anymore becuase all the tools implementations are now outdated.
include($$PWD/../../../uitools/toolkitcpp.pri)
include($$PWD/../../shared.pri)
