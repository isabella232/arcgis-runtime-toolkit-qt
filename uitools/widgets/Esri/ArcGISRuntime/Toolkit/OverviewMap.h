/*******************************************************************************
 *  Copyright 2012-2021 Esri
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
#ifndef ESRI_ARCGISRUNTIME_OVERVIEWMAP_H
#define ESRI_ARCGISRUNTIME_OVERVIEWMAP_H

#include <QWidget>

namespace Ui {
  class OverviewMap;
}

namespace Esri
{
namespace ArcGISRuntime
{

class MapGraphicsView;
class SceneGraphicsView;

namespace Toolkit
{

class OverviewMapController;

class OverviewMap : public QWidget
{
  Q_OBJECT
public:
  explicit OverviewMap(QWidget* parent = nullptr);

  ~OverviewMap() override;

  void setGeoView(MapGraphicsView* mapView);

  void setGeoView(SceneGraphicsView* mapView);

  OverviewMapController* controller() const;

private:
  Ui::OverviewMap* m_ui;
  OverviewMapController* m_controller = nullptr;
};

} // Toolkit
} // ArcGISRuntime
} // Esri

#endif // ESRI_ARCGISRUNTIME_OVERVIEWMAP_H
