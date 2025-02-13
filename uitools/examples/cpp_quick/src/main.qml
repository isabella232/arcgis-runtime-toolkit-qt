// Copyright 2021 ESRI
//
// All rights reserved under the copyright laws of the United States
// and applicable international laws, treaties, and conventions.
//
// You may freely redistribute and use this sample code, with or
// without modification, provided you include the original copyright
// notice and use restrictions.
//
// See the Sample code usage restrictions document for further information.
//

import QtQuick 2.12
import QtQuick.Controls 2.12

ApplicationWindow {
    id: appWindow
    width: 800
    height: 600
    title: "C++ Quick Toolkit Demo App"
    visible: true
    DemoApp {
      anchors.fill: parent
    }
}
