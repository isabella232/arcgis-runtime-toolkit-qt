# Copyright 2012-2022 Esri
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  \
    tst_basemapgalleryfunctest.cpp

HEADERS += \
    tst_basemapgalleryfunctest.h \
    tst_basemapgalleryfunctest.h

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
