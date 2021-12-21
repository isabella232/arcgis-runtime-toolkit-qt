#include "tst_basemapgalleryunittest.h"

#include "AutoDisconnector.h"
#include "Basemap.h"
#include "BasemapGalleryController.h"
#include "Map.h"

#include <ArcGISRuntimeEnvironment.h>
using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;

BasemapGalleryUnitTest::BasemapGalleryUnitTest()
{
}

BasemapGalleryUnitTest::~BasemapGalleryUnitTest()
{

}

void BasemapGalleryUnitTest::initTestCase()
{
  //setting API key

  const QString apiKey = qgetenv("ARCGIS_RUNTIME_API_KEY");
  Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);

  m_basemapLightGray = Basemap::lightGrayCanvas(this);
  m_map = new Map(m_basemapLightGray);

  QSignalSpy mapLoaded(m_map, &Esri::ArcGISRuntime::GeoModel::doneLoading);
  m_map->load();
  AutoDisconnector ad1(connect(m_map, &Esri::ArcGISRuntime::Map::doneLoading, this, [](Esri::ArcGISRuntime::Error loadError)
                               {
                                 QVERIFY2(loadError.isEmpty(), qPrintable(loadError.message()));
                               }));

  QVERIFY2(mapLoaded.wait(), "problem in loading the map.");
}

void BasemapGalleryUnitTest::cleanupTestCase()
{
  delete m_map;
}

void BasemapGalleryUnitTest::ctor_GeoModel()
{
  BasemapGalleryController controller(this);
  QSignalSpy geoModelChanged(&controller, &Esri::ArcGISRuntime::Toolkit::BasemapGalleryController::geoModelChanged);
  controller.setGeoModel(m_map);
  QVERIFY(geoModelChanged.wait() || geoModelChanged.count() > 0);
  QCOMPARE(controller.geoModel(), m_map);
  QCOMPARE(controller.currentBasemap(), m_basemapLightGray);
  auto gallery = controller.gallery();
  QVERIFY(gallery->rowCount() > 0);

  QSet<QString> setBasemapNames;
  for (int row = 0; row < gallery->rowCount(); ++row)
  {
    setBasemapNames.insert(gallery->element<BasemapGalleryItem>(gallery->index(row))->name());
  }
  QSet<QString> setBasemapNamesExpected{"Charted Territory", "Colored Pencil", "Community", "Dark Gray Canvas", "Imagery", "Imagery Hybrid", "Light Gray Canvas", "Mid-Century", "Modern Antique", "Navigation", "Navigation (Night)", "Newspaper", "Nova", "Oceans", "OpenStreetMap", "OpenStreetMap (Dark Gray Canvas)", "OpenStreetMap (Light Gray Canvas)", "OpenStreetMap (Streets with Relief)", "OpenStreetMap (Streets)", "OpenStreetMap (with Relief)", "Streets", "Streets (Night)", "Streets (with Relief)", "Terrain with Labels", "Topographic"};
  QCOMPARE(setBasemapNames, setBasemapNamesExpected);
  QVERIFY(controller.geoModel()->loadStatus() == LoadStatus::Loaded);
}

void BasemapGalleryUnitTest::test1()
{
  QSet<QString> set1{"asd", "lol"};
  QSet<QString> set2{"lol", "asd"};
  QCOMPARE(set1, set2);
}
QTEST_MAIN(BasemapGalleryUnitTest)
