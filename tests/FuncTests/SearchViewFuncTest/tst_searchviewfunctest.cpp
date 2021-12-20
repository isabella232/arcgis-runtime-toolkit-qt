// add necessary includes here
#include "tst_searchviewfunctest.h"

#include "AutoDisconnector.h"
#include "EnvelopeBuilder.h"
#include "GeometryEngine.h"
#include "LocatorSearchSource.h"
#include "LocatorTask.h"
#include "Map.h"
#include "MapGraphicsView.h"
#include "MapQuickView.h"
#include "SearchViewController.h"

#include <ArcGISRuntimeEnvironment.h>

using namespace Esri::ArcGISRuntime::Toolkit;
using namespace Esri::ArcGISRuntime;

/* set all the needed common variables. this signal is run only once at the beginning.
 */
void SearchViewFuncTest::initTestCase()
{
  //setting API key

  const QString apiKey = qgetenv("ARCGIS_RUNTIME_API_KEY");
  Esri::ArcGISRuntime::ArcGISRuntimeEnvironment::setApiKey(apiKey);

  m_locatorTask = new Esri::ArcGISRuntime::LocatorTask(QUrl("https://geocode-api.arcgis.com/arcgis/rest/services/World/GeocodeServer"), nullptr, nullptr);
  init();
  setDataTests();
}

/* resetting to inital state everything except m mapview and locatortask. they are being reused since they are not changed too much (viewpoint mostly).
 * todo: chek that the locatortask doesnt need to be reset as well.
 */
void SearchViewFuncTest::init()
{
  //reset the controller
  delete controller;
  controller = new SearchViewController(this);
  controller->sources()->clear();

  LocatorSearchSource* newLocatorSource = new LocatorSearchSource(m_locatorTask);
  m_locatorTask->setParent(newLocatorSource); //pr.setting new parent to locator task before deleting the current parent (worried about being deleted along with the parent)
  delete m_locatorSource;
  m_locatorSource = newLocatorSource;

  controller->sources()->append(m_locatorSource);
  controller->setActiveSource(m_locatorSource);
}

/* clean up all the resources
 */
void SearchViewFuncTest::cleanupTestCase()
{
  delete m_locatorTask; //delete before than the source.
  delete m_locatorSource;
  delete controller;
}

void SearchViewFuncTest::setDataTests()
{
  auto sr(SpatialReference::wgs84());
  PolygonBuilder builder(sr, this);
  builder.addPoint(Point(-91.59127653822401, 44.74770908213401, sr));
  builder.addPoint(Point(-91.19322516572637, 44.74770908213401, sr));
  builder.addPoint(Point(-91.19322516572637, 45.116100854348254, sr));
  builder.addPoint(Point(-91.19322516572637, 45.116100854348254, sr));
  chippewaFalls = Polygon(builder.toGeometry());

  //minneapolis
  PolygonBuilder builderminn(sr, this);
  builderminn.addPoint(Point(-94.170821328662, 44.13656401114444, sr));
  builderminn.addPoint(Point(-94.170821328662, 44.13656401114444, sr));
  builderminn.addPoint(Point(-92.34544467133114, 45.824325577904446, sr));
  builderminn.addPoint(Point(-92.34544467133114, 45.824325577904446, sr));
  minneapolis = Polygon(builderminn.toGeometry());

  portland = Point(-122.658722, 45.512230, sr);
  edinburgh = Point(-3.188267, 55.953251, sr);
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
                            //qDebug() << "selecting suggestion, " << firstSuggestion->displayTitle();
                            controller->acceptSuggestion(firstSuggestion);
                          }
                        });

  QSignalSpy selectedResultSpy(controller, &Esri::ArcGISRuntime::Toolkit::SearchViewController::selectedResultChanged);
  QSignalSpy rowsInserted(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted);
  controller->setCurrentQuery("Magers & Quinn Booksellers");

  QVERIFY(selectedResultSpy.wait(10000));
  QEXPECT_FAIL("", "rowsInserted for results() is never called in case of direct suggestion accept. There is difference in the test design and toolkit implementation.", Abort);
  QVERIFY(rowsInserted.wait(5000)); //
}

void SearchViewFuncTest::activeSource_1_2_1()
{
  m_locatorSource->setDisplayName("Simple Locator");
  QSignalSpy completeSpy(this, &SearchViewFuncTest::waitThis);
  QVERIFY(controller->activeSource()->displayName() == "Simple Locator");
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 qDebug() << searchResults.count();
                                 if (searchResults.count() == 0)
                                   QVERIFY(false);
                                 else
                                   QVERIFY2(searchResults.first()->owningSource()->displayName() == "Simple Locator", qPrintable(searchResults.first()->owningSource()->displayName()));
                                 emit waitThis();
                               }));
  controller->setCurrentQuery(magersBooksellers);
  controller->commitSearch(true);
  QVERIFY(completeSpy.wait());
}

void SearchViewFuncTest::activeSource_1_2_2()
{
  m_locatorSource->setDisplayName("Simple Locator");
  QSignalSpy suggestComplete(controller->activeSource()->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted);
  AutoDisconnector ad1(connect(controller->activeSource()->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted, this, [this]()
                               {
                                 auto suggestions = this->controller->suggestions();
                                 QVERIFY(suggestions->rowCount() > 0);
                                 QCOMPARE(suggestions->element<SearchSuggestion>(suggestions->index(0))->owningSource()->displayName(), "Simple Locator");
                               }));
  controller->setCurrentQuery(magersBooksellers);
  QVERIFY(suggestComplete.wait(10000));
}

void SearchViewFuncTest::commitSearch_1_3_1()
{
  QCOMPARE(controller->results()->rowCount(), 0);
}

void SearchViewFuncTest::commitSearch_1_3_2()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 QCOMPARE(controller->results()->rowCount(), 0);
                                 QCOMPARE(searchResults.count(), 0);
                                 emit waitThis();
                               }));
  controller->setCurrentQuery("No results found blah blah blah blah");
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::commitSearch_1_3_3()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  QSignalSpy rowsInserted(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted);

  AutoDisconnector ad1(connect(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted, this, [this]()
                               {
                                 QCOMPARE(controller->results()->element<SearchResult>(controller->results()->index(0)), controller->selectedResult());
                               }));
  AutoDisconnector ad2(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 QVERIFY(controller->selectedResult() != nullptr);
                                 emit waitThis();
                               }));
  controller->setCurrentQuery(magersBooksellers); //only 1 result is shown: when calling commit search, no results are added, but just selected the single one from the locatorsource.
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
  QEXPECT_FAIL("", "the test design expects the result to be addded into the results(), but in the Qt implementation, it is only added in the selectedResult()", Abort);
  QVERIFY(rowsInserted.wait(1000));
}

void SearchViewFuncTest::commitSearch_1_3_4()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);

  AutoDisconnector ad1(connect(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted, this, [this]()
                               {
                                 QCOMPARE(controller->results()->element<SearchResult>(controller->results()->index(0)), controller->selectedResult());
                               }));
  AutoDisconnector ad2(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 QVERIFY(controller->results()->rowCount() > 1);
                                 QVERIFY(controller->selectedResult() == nullptr);
                                 emit waitThis();
                               }));
  controller->setCurrentQuery(magersQuinn);
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::currentQuery_1_4_1()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  QSignalSpy rowsInsertedResults(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted);
  QSignalSpy rowsInsertedSuggestions(controller->suggestions(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted);

  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 QCOMPARE(controller->results()->rowCount(), 0);
                                 QCOMPARE(controller->suggestions()->rowCount(), 0);
                                 emit waitThis();
                               }));
  controller->setCurrentQuery({});
  controller->commitSearch(true);
  QVERIFY(!rowsInsertedResults.wait(2000));
  //don't need to wait, shoudl have been fired while waiting for the results signal.
  QCOMPARE(rowsInsertedSuggestions.count(), 0);
  //if the signal was fired while waiting for the previous failing one, it is stored in the count, so check it
  QVERIFY(searchComplete.count() > 0 || searchComplete.wait());
}

void SearchViewFuncTest::currentQuery_1_4_2()
{
  QSignalSpy rowsInsertedResults(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted);
  controller->setCurrentQuery(coffee);
  controller->commitSearch(true);
  AutoDisconnector ad1(connect(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsInserted, this, [this]()
                               {
                                 QVERIFY(controller->results()->rowCount() > 0);
                               }));

  QVERIFY(rowsInsertedResults.wait());
}

void SearchViewFuncTest::currentQuery_1_4_3()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  QSignalSpy rowsRemovedResults(controller->results(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsRemoved);
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 // changing the query after first has been processed.
                                 controller->setCurrentQuery(QStringLiteral("Coffee in Portland"));
                               }));
  controller->setCurrentQuery(coffee);
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
  QEXPECT_FAIL("", "searchview design is different from qt implementation: in qt, after a change of query, the results are not cleared. They are on IOS.", Abort);
  QVERIFY(rowsRemovedResults.wait());
}

void SearchViewFuncTest::currentQuery_1_4_4()
{
  QSignalSpy suggestComplete(controller->activeSource()->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted);

  AutoDisconnector ad1(connect(controller->activeSource()->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted, this, [this]()
                               {
                                 QVERIFY(controller->suggestions()->rowCount() > 0);
                               }));
  controller->setCurrentQuery(coffee);
  QVERIFY(suggestComplete.wait());
}

void SearchViewFuncTest::currentQuery_1_4_5()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  QSignalSpy rowsRemovedSuggestions(controller->suggestions(), &Esri::ArcGISRuntime::Toolkit::GenericListModel::rowsRemoved);
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 // changing the query after first has been processed. This will trigger to remove the suggestions.
                                 controller->setCurrentQuery(QStringLiteral("Coffee in Portland"));
                               }));

  controller->setCurrentQuery("Rome");
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
  QVERIFY(rowsRemovedSuggestions.wait());
}

void SearchViewFuncTest::currentQuery_1_4_7()
{
  QSignalSpy selectedResultChanged(controller, &Esri::ArcGISRuntime::Toolkit::SearchViewController::selectedResultChanged);
  controller->setCurrentQuery(magersBooksellers);
  controller->commitSearch(true);
  //wait for first selected result to be set.
  QVERIFY(selectedResultChanged.wait());
  controller->setCurrentQuery(QStringLiteral("Hotel"));
  AutoDisconnector ad1(connect(controller, &Esri::ArcGISRuntime::Toolkit::SearchViewController::selectedResultChanged, this, [this]()
                               {
                                 QVERIFY(controller->selectedResult() == nullptr);
                               }));
  QEXPECT_FAIL("", "current Qt implementation will not reset the selectedResult when changing the currentQuery", Continue);
  QVERIFY2(selectedResultChanged.wait() || selectedResultChanged.count() > 1, qPrintable("current selected result title: " + controller->selectedResult()->displayTitle()));
  selectedResultChanged.wait(1000);
}

void SearchViewFuncTest::isEligibleForRequery_1_5_1()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  controller->setQueryArea(chippewaFalls);
  if (auto geoview = qobject_cast<GeoView*>(this->controller->geoView()))
    geoview->setViewpoint(chippewaFalls.extent());
  controller->setCurrentQuery(coffee);
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                               }));

  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
  QCOMPARE(controller->isEligableForRequery(), false);
}

void SearchViewFuncTest::isEligibleForRequery_1_5_2()
{
  //  QSignalSpy searchComplete(this, S&SearchViewFuncTest::waitThis);
  //  QSignalSpy isEnabledChanged(controller, &Esri::ArcGISRuntime::Toolkit::SearchViewController::isEligableForRequeryChanged);

  //  controller->setQueryArea(chippewaFalls);
  //  auto geoview = qobject_cast<GeoView*>(this->controller->geoView());
  //  geoview->setViewpoint(chippewaFalls.extent());
  //  controller->setCurrentQuery(coffee);
  //  //auto enveBuilder = std::make_shared<EnvelopeBuilder>(chippewaFalls.extent(), this);
  //  EnvelopeBuilder* enveBuilder = new EnvelopeBuilder(chippewaFalls.extent(), this);
  //  auto geoQuickView = qobject_cast<MapQuickView*>(this->controller->geoView());

  //  connect(geoQuickView, &Esri::ArcGISRuntime::MapQuickView::viewpointChanged, this, []()
  //          {
  //            qDebug() << "changed";
  //          });

  //  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this, enveBuilder, geoview](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
  //                               {
  //                                 QSignalSpy viewpointChanged(m_mapView, &Esri::ArcGISRuntime::MapQuickView::viewpointChanged);
  //                                 emit waitThis();
  //                                 //qDebug() << enveBuilder->toEnvelope().toJson();
  //                                 enveBuilder->expandByFactor(10.0);

  //                                 qDebug() << "setting the viuewqpoitn";

  //                                 qDebug() << enveBuilder->toEnvelope().toJson();
  //                                 geoview->setViewpoint(enveBuilder->toEnvelope());
  //                                 QVERIFY(viewpointChanged.wait());
  //                               }));

  //  controller->commitSearch(true);
  //  QVERIFY(searchComplete.wait(15000));
  //  //QTest::qWait(10000);

  //  //QVERIFY(viewpointChanged.count() > 1);
  //  QCOMPARE(controller->isEligableForRequery(), false);
}

void SearchViewFuncTest::isEligibleForRequery_1_5_3()
{
  //  controller->setQueryArea(chippewaFalls);
  //  auto geoview = qobject_cast<GeoView*>(this->controller->geoView());
  //  geoview->setViewpoint(chippewaFalls.extent());
  //  controller->setCurrentQuery(coffee);
  //  auto enveBuilder = std::make_shared<EnvelopeBuilder>(chippewaFalls.extent(), this);
  //  enveBuilder->expandByFactor(50);
}

void SearchViewFuncTest::queryArea_1_6_1()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  m_locatorSource->setMaximumResults(std::numeric_limits<int>::max());
  controller->setQueryArea(chippewaFalls);
  controller->setCurrentQuery(coffee);

  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 QVERIFY(controller->results()->rowCount() > 0);
                               }));
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::queryArea_1_6_2()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  m_locatorSource->setMaximumResults(std::numeric_limits<int>::max());
  controller->setQueryArea(chippewaFalls);
  controller->setCurrentQuery(coffee);

  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 QList<Geometry> l;
                                 for (auto result : searchResults)
                                 {
                                   l.append(result->geoElement()->geometry());
                                 }
                                 qDebug() << searchResults.count();
                                 QVERIFY(GeometryEngine::contains(GeometryEngine::unionOf(l), this->controller->queryArea()));
                               }));
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::queryArea_1_6_3()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  controller->setQueryArea(chippewaFalls);
  controller->setCurrentQuery(magersBooksellers);

  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 QCOMPARE(controller->results()->rowCount(), 0);
                               }));
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::queryArea_1_6_4()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  controller->setQueryArea(minneapolis);
  controller->setCurrentQuery(magersBooksellers);

  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 qDebug() << searchResults.count();
                                 QCOMPARE(searchResults.count(), 1);
                                 QEXPECT_FAIL("", "If search outputs a single result, it is automatically selected, but not added to the results genericmodellist", Abort);
                                 QCOMPARE(controller->results()->rowCount(), 1);
                               }));
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::queryCenter_1_7_1()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  controller->setQueryCenter(portland);
  controller->setCurrentQuery(coffee);
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 auto geometryResult = searchResults.first()->geoElement()->geometry();
                                 auto distanceGeom = GeometryEngine::distanceGeodetic(portland, geometryResult, LinearUnit(LinearUnitId::Meters), AngularUnit(), GeodeticCurveType::Geodesic);
                                 QVERIFY(distanceGeom.distance() < 1500.0);
                               }));

  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::queryCenter_1_7_2()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  controller->setQueryCenter(edinburgh);
  controller->setCurrentQuery("Restaurants");

  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 auto geometryResult = searchResults.first()->geoElement()->geometry();
                                 auto distanceGeom = GeometryEngine::distanceGeodetic(edinburgh, geometryResult, LinearUnit(LinearUnitId::Meters), AngularUnit(), GeodeticCurveType::Geodesic);
                                 QVERIFY(distanceGeom.distance() < 100.0);
                               }));
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::searchResultMode_1_9_1()
{
  QVERIFY(controller->resultMode() == SearchViewController::SearchResultMode::Automatic);
}

void SearchViewFuncTest::searchResultMode_1_9_2()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  controller->setResultMode(SearchViewController::SearchResultMode::Single);
  controller->setCurrentQuery(magersQuinn);
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 QEXPECT_FAIL("", "If search outputs a single result, it is automatically selected, but not added to the results genericmodellist", Abort);
                                 QCOMPARE(controller->results()->rowCount(), 1);
                               }));

  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::searchResultMode_1_9_3()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  controller->setResultMode(SearchViewController::SearchResultMode::Multiple);
  controller->setCurrentQuery(magersQuinn);
  AutoDisconnector ad1(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 QVERIFY(controller->results()->rowCount() > 1);
                               }));

  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::searchResultMode_1_9_4()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  auto connection = std::make_shared<QMetaObject::Connection>();
  *connection = connect(controller->suggestions(), &GenericListModel::rowsInserted, this, [this, connection]()
                        {
                          auto suggestions = this->controller->suggestions();
                          SearchSuggestion *trueCollection = nullptr, *falseCollection = nullptr;

                          for (int i = 0; i < suggestions->rowCount(); ++i)
                          {
                            QModelIndex index = suggestions->index(i);
                            auto s = suggestions->element<SearchSuggestion>(index);
                            //true collection
                            if (!trueCollection && s->isCollection())
                              trueCollection = s;
                            //false collection
                            if (!falseCollection && !s->isCollection())
                              falseCollection = s;
                          }
                          if (trueCollection)
                          {
                            disconnect(*connection);
                            controller->acceptSuggestion(trueCollection);
                          }
                        });
  AutoDisconnector ad(connect(controller->activeSource(), &Esri::ArcGISRuntime::Toolkit::SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                              {
                                emit waitThis();
                                QVERIFY(controller->results()->rowCount() > 1);
                              }));
  controller->setResultMode(SearchViewController::SearchResultMode::Automatic);
  controller->setCurrentQuery(coffee);

  QVERIFY(searchComplete.wait());
  controller->commitSearch(true);
}

void SearchViewFuncTest::searchResultMode_1_9_5()
{
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  auto connection = std::make_shared<QMetaObject::Connection>();
  *connection = connect(controller->suggestions(), &GenericListModel::rowsInserted, this, [this, connection]()
                        {
                          auto suggestions = this->controller->suggestions();
                          SearchSuggestion *trueCollection = nullptr, *falseCollection = nullptr;

                          for (int i = 0; i < suggestions->rowCount(); ++i)
                          {
                            QModelIndex index = suggestions->index(i);
                            auto s = suggestions->element<SearchSuggestion>(index);
                            //true collection
                            if (!trueCollection && s->isCollection())
                              trueCollection = s;
                            //false collection
                            if (!falseCollection && !s->isCollection())
                              falseCollection = s;
                          }
                          if (falseCollection)
                          {
                            disconnect(*connection);
                            controller->acceptSuggestion(falseCollection);
                            qDebug() << controller->results()->rowCount();
                            ;
                          }
                        });
  connect(this->controller, &SearchViewController::selectedResultChanged, this, [this]()
          {
            emit waitThis();
            QVERIFY(controller->selectedResult() != nullptr);
            QEXPECT_FAIL("", "", Abort);
            QCOMPARE(controller->results()->rowCount(), 1);
          });
  controller->setResultMode(SearchViewController::SearchResultMode::Automatic);
  controller->setCurrentQuery("Coffee");
  controller->commitSearch(true);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::maximumResults_2_1_1()
{
  m_locatorSource->setMaximumResults(4);
  QCOMPARE(m_locatorSource->maximumResults(), 4);
}

void SearchViewFuncTest::maximumResults_2_1_2()
{
  m_locatorSource->setMaximumResults(4);
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);
  AutoDisconnector ad1(connect(m_locatorSource, &SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 QCOMPARE(searchResults.count(), 4);
                               }));
  m_locatorSource->search(coffee);
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::maximumResults_2_1_3()
{
  m_locatorSource->setMaximumResults(4);
  QSignalSpy searchComplete(this, &SearchViewFuncTest::waitThis);

  m_locatorSource->search(coffee);

  { // inner scope to automatically destroy the connection after the successful qverify check
    AutoDisconnector ad1(connect(m_locatorSource, &SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                                 {
                                   emit waitThis();
                                 }));

    QVERIFY(searchComplete.wait()); //consume first search call
  }
  AutoDisconnector ad1(connect(m_locatorSource, &SearchSourceInterface::searchCompleted, this, [this](QList<Esri::ArcGISRuntime::Toolkit::SearchResult*> searchResults)
                               {
                                 emit waitThis();
                                 QCOMPARE(searchResults.count(), 12);
                               }));
  m_locatorSource->setMaximumResults(12);
  m_locatorSource->search("Restaurant");
  QVERIFY(searchComplete.wait());
}

void SearchViewFuncTest::maximumSuggestions_2_2_1()
{
  m_locatorSource->setMaximumSuggestions(4);
  QCOMPARE(m_locatorSource->maximumSuggestions(), 4);
}

void SearchViewFuncTest::maximumSuggestions_2_2_2()
{
  m_locatorSource->setMaximumSuggestions(4);
  QSignalSpy suggestComplete(m_locatorSource->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted);
  AutoDisconnector ad1(connect(m_locatorSource->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted, this, [this]()
                               {
                                 QCOMPARE(m_locatorSource->suggestions()->rowCount(), 4);
                               }));
  m_locatorSource->suggestions()->setSearchText(coffee);
  QVERIFY(suggestComplete.wait());
}

void SearchViewFuncTest::maximumSuggestions_2_2_3()
{
  QSignalSpy suggestComplete(m_locatorSource->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted);
  m_locatorSource->setMaximumSuggestions(4);
  m_locatorSource->suggestions()->setSearchText(coffee);

  m_locatorSource->setMaximumSuggestions(12);
  QVERIFY(suggestComplete.wait()); //consume first suggest call

  m_locatorSource->suggestions()->setSearchText(coffee);
  AutoDisconnector ad1(connect(m_locatorSource->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted, this, [this]()
                               {
                                 QCOMPARE(m_locatorSource->suggestions()->rowCount(), 12);
                               }));
  QVERIFY(suggestComplete.wait());
}

void SearchViewFuncTest::maximumSuggestions_2_2_4()
{
  QSignalSpy suggestComplete(m_locatorSource->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted);
  auto sp = Esri::ArcGISRuntime::SuggestParameters();
  sp.setMaxResults(2);
  m_locatorSource->suggestions()->setSuggestParameters(sp);
  QVERIFY(m_locatorSource->maximumSuggestions());

  AutoDisconnector ad1(connect(m_locatorSource->suggestions(), &Esri::ArcGISRuntime::SuggestListModel::suggestCompleted, this, [this]()
                               {
                                 QCOMPARE(m_locatorSource->suggestions()->rowCount(), 2);
                               }));
  m_locatorSource->suggestions()->setSearchText(coffee);
  QVERIFY(suggestComplete.wait());
}

QTEST_MAIN(SearchViewFuncTest)
