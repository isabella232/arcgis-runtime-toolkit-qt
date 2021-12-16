// add necessary includes here
#include "tst_searchviewfunctest.h"

#include "AutoDisconnector.h"
#include "LocatorSearchSource.h"
#include "LocatorTask.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "MapQuickView.h"
#include "SearchViewController.h"

#include <ArcGISRuntimeEnvironment.h>

using namespace Esri::ArcGISRuntime::Toolkit;
using namespace Esri::ArcGISRuntime;

/* set all the needed common variables
 */
void SearchViewFuncTest::initTestCase()
{
  //setting API key

  const QString apiKey = qgetenv("ARCGIS_RUNTIME_API_KEY");
  Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);

  auto mapView = new Esri::ArcGISRuntime::MapQuickView(new Map(BasemapStyle::ArcGISCommunity, this));

  controller = new SearchViewController(this);
  controller->setGeoView(mapView);
  controller->sources()->clear();
  start = std::chrono::system_clock::now();
  controller->sources()->clear();
  m_locatorTask = new Esri::ArcGISRuntime::LocatorTask(QUrl("https://geocode-api.arcgis.com/arcgis/rest/services/World/GeocodeServer"), nullptr, nullptr);
  m_locatorSource = new LocatorSearchSource(m_locatorTask);
  m_locatorTask->setParent(m_locatorSource);
  controller->sources()->append(m_locatorSource);
  controller->setActiveSource(m_locatorSource);
  //wait that the map is loaded
  QSignalSpy(mapView->map(), SIGNAL(doneLoading(Esri::ArcGISRuntime::Error))).wait(1000);
}

void SearchViewFuncTest::acceptSuggestion_1_1_1()
{
  // failing. results are not added when selected directly from a suggestion. wanted behaviour?
  m_locatorSource->setDisplayName("Simple Locator");

  connect(controller, &Esri::ArcGISRuntime::Toolkit::SearchViewController::selectedResultChanged, this, [this]()
          {
            QVERIFY(controller->selectedResult() != nullptr);
          });

  connect(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted, this, [this]()
          {
            qDebug() << "search completed";
            qDebug() << "# results: ";
            qDebug() << controller->results()->rowCount();
            QVERIFY(controller->results()->rowCount() == 1);
            QVERIFY(controller->selectedResult() != nullptr);
            QVERIFY(controller->suggestions()->rowCount() == 0);
          });
  //Q_UNUSED(ad1);

  QMetaObject::Connection* connection = new QMetaObject::Connection();
  *connection = connect(controller->suggestions(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted, this, [this, connection]()
                        {
                          disconnect(*connection);
                          //delete connection;
                          auto suggestions = controller->suggestions();
                          if (suggestions->rowCount() > 0)
                          {
                            auto firstSuggestion = suggestions->element<SearchSuggestion>(suggestions->index(0));
                            qDebug() << "selecting suggestion, " << firstSuggestion->displaySubtitle();
                            controller->acceptSuggestion(firstSuggestion);
                          }
                        });
  controller->setCurrentQuery("Rome");

  //waiting for the completed signal. It seems that without waitForSignal, the search function is not completed.
  QSignalSpy completeSpy(controller->results(), SIGNAL(rowsInserted()));
  QVERIFY(completeSpy.wait(5000));
  QSignalSpy selectedResultSpy(controller->selectedResult(), SIGNAL(selectedResultChanged()));
  QVERIFY(selectedResultSpy.wait(1000));
}

void SearchViewFuncTest::activeSource_1_2_1()
{
  QSignalSpy completeSpy(this, SIGNAL(waitThis()));
  m_locatorSource->setDisplayName("Simple Locator");
  QVERIFY(controller->activeSource()->displayName() == "Simple Locator");
  connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
          {
            //qDebug() << controller->results()->rowCount();
            //            if (searchResults.count() == 0)
            //              VERIFY(false);
            //            else
            //              VERIFY(searchResults.first()->owningSource()->displayName() == "Simple Locator");
            emit waitThis();
          });
  controller->setCurrentQuery(magersBooksellers);
  controller->commitSearch(true);
  QVERIFY(completeSpy.wait());
}

QTEST_MAIN(SearchViewFuncTest)
