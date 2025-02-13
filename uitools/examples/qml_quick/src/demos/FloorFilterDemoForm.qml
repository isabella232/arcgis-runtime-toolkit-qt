/*******************************************************************************
 *  Copyright 2012-2022 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/
import QtQuick 2.12
import QtQuick.Controls 2.12
import Esri.ArcGISRuntime 100.14
import Esri.ArcGISRuntime.Toolkit 100.14

DemoPage {
    mapViewContents: Component {
        //! [Set up Floor Filter QML]
        MapView {
            id: viewMap
            Map {
                initUrl: "https://www.arcgis.com/home/item.html?id=f133a698536f44c8884ad81f80b6cfc7"
            }
            FloorFilter {
                geoView: parent
                anchors {
                    left: parent.left
                    margins: 10
                    top: parent.top
                }
            }
        }

        //! [Set up Floor Filter QML]
    }
}
