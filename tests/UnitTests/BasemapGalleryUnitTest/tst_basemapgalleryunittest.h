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
  void test1();

private:
  Esri::ArcGISRuntime::Map* m_map;
  Esri::ArcGISRuntime::Basemap* m_basemapLightGray;
};

#endif // TST_BASEMAPGALLERYUNITTEST_H
