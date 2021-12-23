#include "tst_basemapgalleryunittest.h"

#include "AutoDisconnector.h"
#include "Basemap.h"
#include "BasemapGalleryController.h"
#include "Map.h"
#include "SignalSynchronizer.h"

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
  delete m_basemapLightGray;
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

//todo: find out how to get the loaded basemaps from the portal and the gallery. atm are not all loaded when checking
void BasemapGalleryUnitTest::ctor_GeoModelPortal()
{
  BasemapGalleryController controller(this);
  QSignalSpy geoModelChanged(&controller, &Esri::ArcGISRuntime::Toolkit::BasemapGalleryController::geoModelChanged);
  controller.setGeoModel(m_map);
  QVERIFY(geoModelChanged.wait() || geoModelChanged.count() > 0);
  QCOMPARE(controller.geoModel(), m_map);
  QCOMPARE(controller.geoModel()->basemap(), m_basemapLightGray);
  auto portal = std::make_shared<Portal>(QUrl("https://www.arcgis.com"), this);

  QSignalSpy portalLoaded(portal.get(), &Esri::ArcGISRuntime::Portal::doneLoading);
  QSignalSpy portalBasemapChanged(portal.get(), &Esri::ArcGISRuntime::Portal::basemapsChanged);

  auto gallery = controller.gallery();
  // portal is loaded and basemaps are fetched and loaded by the controller
  controller.setPortal(portal.get());
  QVERIFY(portalLoaded.count() > 0 || portalLoaded.wait());
  QVERIFY(portalBasemapChanged.count() > 0 || portalBasemapChanged.wait());
  // add all the basemaps signal loadstatuschanged in the signalsync list.
  SignalSynchronizer signalSync(portal->basemaps()->begin(), portal->basemaps()->end(), &Basemap::loadStatusChanged, this);
  QSignalSpy signalSyncReady(&signalSync, &SignalSynchronizer::ready);
  QVERIFY(signalSyncReady.count() > 0 || signalSyncReady.wait());

  QCOMPARE(controller.portal(), portal.get());
  size_t basemapsLength = portal->basemaps()->rowCount();
  QVERIFY(gallery->rowCount() == basemapsLength && basemapsLength > 0);
  //have to use a set because the gallery items are ordered by name
  QSet<QString> setBasemapNamesGallery;
  QSet<QString> setBasemapNamesPortal;
  for (int row = 0; row < basemapsLength; ++row)
  {
    setBasemapNamesGallery.insert(gallery->element<BasemapGalleryItem>(gallery->index(row))->name());
    setBasemapNamesPortal.insert(portal->basemaps()->at(row)->name());
  }
  QCOMPARE(setBasemapNamesGallery, setBasemapNamesPortal);
}

void BasemapGalleryUnitTest::ctor_GeoModelBasemapGalleryItems()
{
  BasemapGalleryController controller(this);
  QSignalSpy geoModelChanged(&controller, &Esri::ArcGISRuntime::Toolkit::BasemapGalleryController::geoModelChanged);
  controller.setGeoModel(m_map);

  QVERIFY(geoModelChanged.wait() || geoModelChanged.count() > 0);
  //removing all rows (mimick clean gallery state). by waiting the geomodel is loaded, the gallery is loaded as well
  controller.gallery()->clear();
  //basemap loaded in the call-stack of append.
  Basemap* b1 = new Basemap(new PortalItem(QUrl("https://runtime.maps.arcgis.com/home/item.html?id=f33a34de3a294590ab48f246e99958c9")));
  QSignalSpy b1Loaded(b1, &Basemap::loadStatusChanged);
  QCOMPARE(controller.gallery()->rowCount(), 0);
  controller.append(b1);
  QVERIFY(b1Loaded.wait());
  Basemap* b2 = new Basemap(new PortalItem(QUrl("https://runtime.maps.arcgis.com/home/item.html?id=46a87c20f09e4fc48fa3c38081e0cae6")));
  QSignalSpy b2Loaded(b2, &Basemap::loadStatusChanged);
  controller.append(b2);
  QVERIFY(b2Loaded.wait());
  QCOMPARE(controller.currentBasemap(), m_map->basemap());
  auto gallery = controller.gallery();
  QSet<QString> setBasemapNames;
  for (int row = 0; row < gallery->rowCount(); ++row)
  {
    QString name = gallery->element<BasemapGalleryItem>(gallery->index(row))->name();
    QVERIFY(name != "");
    setBasemapNames.insert(name);
  }
  QSet<QString> setBasemapNamesExpected{b1->name(), b2->name()};
  QCOMPARE(setBasemapNames, setBasemapNamesExpected);

  //free resources
  delete b1;
  delete b2;
}

QTEST_MAIN(BasemapGalleryUnitTest)
