#-------------------------------------------------
#  Copyright 2022 ESRI
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

CONFIG += c++14 warn_on qmltestcase

TEMPLATE = app

ARCGIS_RUNTIME_VERSION = 100.13
include($$PWD/../arcgisruntime.pri)
include($$PWD/../../../uitools/toolkitqml.pri)

macx {
    cache()
}

TARGET = qml_quick

equals(QT_MAJOR_VERSION, 5) {
    lessThan(QT_MINOR_VERSION, 15) {
        error("$$TARGET requires Qt 5.15.2")
    }
	equals(QT_MINOR_VERSION, 15) : lessThan(QT_PATCH_VERSION, 2) {
		error("$$TARGET requires Qt 5.15.2")
	}
}

equals(QT_MAJOR_VERSION, 6) {
  error("This version of the ArcGIS Runtime SDK for Qt is incompatible with Qt 6")
}

#-------------------------------------------------------------------------------

DISTFILES += \
    tst_basemapgalleryunittest.qml

SOURCES += \
    tst_main.cpp

RESOURCES += \
    qtquick.qrc \
    $$absolute_path($$PWD/../../../calcite-qml/Calcite/calcite.qrc) \
    
QML2_IMPORT_PATH += $$absolute_path($$PWD/../../../uitools/import)

#missing all the different platform specific imports (win.pri, mac.pri, etc..)

#-------------------------------------------------------------------------------

win32 {
    include (../Win/Win.pri)
}
