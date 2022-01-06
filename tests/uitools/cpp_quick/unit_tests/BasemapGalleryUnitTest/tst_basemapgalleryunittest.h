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
#ifndef TST_BASEMAPGALLERYUNITTEST_H
#define TST_BASEMAPGALLERYUNITTEST_H

#include <QtTest>

namespace Esri {
namespace ArcGISRuntime {
  class Map;
  class Basemap;
  namespace Toolkit {
  }
}
}

class BasemapGalleryUnitTest : public QObject
{
  Q_OBJECT

public:
  BasemapGalleryUnitTest();
  ~BasemapGalleryUnitTest();

private slots:
  void initTestCase();
  void cleanupTestCase();
  void ctor_GeoModel();
  void ctor_GeoModelPortal();
  void ctor_GeoModelBasemapGalleryItems();

signals:
  void waitThis();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::Basemap* m_basemapLightGray;
};

#endif // TST_BASEMAPGALLERYUNITTEST_H
