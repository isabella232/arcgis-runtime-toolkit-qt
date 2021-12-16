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
  QVERIFY(QSignalSpy(mapView->map(), SIGNAL(doneLoading(Esri::ArcGISRuntime::Error))).wait());
}

void SearchViewFuncTest::acceptSuggestion_1_1_1()
{
  // failing. results are not added when selected directly from a suggestion. wanted behaviour?
  AutoDisconnector ad1(connect(controller, &Esri::ArcGISRuntime::Toolkit::SearchViewController::selectedResultChanged, this, [this]()
                               {
                                 QVERIFY(controller->selectedResult() != nullptr);
                                 emit waitThis();
                               }));
  Q_UNUSED(ad1);

  AutoDisconnector ad2(connect(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted, this, [this]()
                               {
                                 qDebug() << "search completed";
                                 qDebug() << "# results: ";
                                 qDebug() << controller->results()->rowCount();
                                 QVERIFY(controller->results()->rowCount() == 1);
                                 QVERIFY(controller->selectedResult() != nullptr);
                                 QVERIFY(controller->suggestions()->rowCount() == 0);
                                 emit waitThis();
                               }));
  Q_UNUSED(ad2);

  QMetaObject::Connection* connection = new QMetaObject::Connection();
  *connection = connect(controller->suggestions(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted, this, [this, connection]()
                        {
                          disconnect(*connection);
                          //delete connection;
                          delete connection;
                          auto suggestions = controller->suggestions();
                          if (suggestions->rowCount() > 0)
                          {
                            auto firstSuggestion = suggestions->element<SearchSuggestion>(suggestions->index(0));
                            qDebug() << "selecting suggestion, " << firstSuggestion->displayTitle();
                            controller->acceptSuggestion(firstSuggestion);
                          }
                        });
  //todo:add rowsInserted spysignal
  //QSignalSpy completeSpy(controller->suggestions(), SIGNAL(rowsInserted(const QModelIndex&, int, int, QAbstractItemModel::QPrivateSignal)));

  QSignalSpy selectedResultSpy(controller, &Esri::ArcGISRuntime::Toolkit::SearchViewController::selectedResultChanged);
  QSignalSpy rowsInserted(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted);
  controller->setCurrentQuery("Magers & Quinn Booksellers");
  //temporary solution: waiting twice for the rowsInserted connections (emitting waitThis in them)
  QVERIFY(selectedResultSpy.wait(10000));
  QVERIFY(rowsInserted.wait(5000)); //second is never called. there is difference in the test design and toolkit implementation.
}

void SearchViewFuncTest::activeSource_1_2_1()
{
  m_locatorSource->setDisplayName("Simple Locator");
  QSignalSpy completeSpy(this, SIGNAL(waitThis()));
  m_locatorSource->setDisplayName("Simple Locator");
  QVERIFY(controller->activeSource()->displayName() == "Simple Locator");
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 qDebug() << searchResults.count();
                                 if (searchResults.count() == 0)
                                   QVERIFY(false);
                                 else
                                   QVERIFY(searchResults.first()->owningSource()->displayName() == "Simple Locator");
                                 qDebug() << searchResults.first()->owningSource()->displayName();
                                 emit waitThis();
                               }));
  controller->setCurrentQuery(magersBooksellers);
  controller->commitSearch(true);
  QVERIFY(completeSpy.wait());
}

void SearchViewFuncTest::activeSource_1_2_2()
{
}

QTEST_MAIN(SearchViewFuncTest)
