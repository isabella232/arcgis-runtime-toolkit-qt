#ifndef TST_SEARCHVIEWFUNCTEST_H
#define TST_SEARCHVIEWFUNCTEST_H

#include "PolygonBuilder.h"

#include <QtTest>

namespace Esri {
namespace ArcGISRuntime {
  class LocatorTask;
  class MapQuickView;

  namespace Toolkit {
    class LocatorSearchSource;
    class SearchViewController;
  }
}
}

class SearchViewFuncTest : public QObject
{
  Q_OBJECT

  // premade strings
  const QString magersBooksellers{QStringLiteral("Magers & Quinn Booksellers")};
  const QString magersQuinn{QStringLiteral("Magers & Quinn")};
  const QString coffee{QStringLiteral("Coffee")};

  //data
  Esri::ArcGISRuntime::Polygon chippewaFalls;
  Esri::ArcGISRuntime::Polygon minneapolis;
  Esri::ArcGISRuntime::Point portland;
  Esri::ArcGISRuntime::Point edinburgh;

public:
  SearchViewFuncTest() = default;
  ~SearchViewFuncTest() = default;

private slots:
  void initTestCase();
  void init(); // clearing preset things from previous tests
  void cleanupTestCase();

  void acceptSuggestion_1_1_1();
  void activeSource_1_2_1();
  void activeSource_1_2_2();
  void commitSearch_1_3_1();
  void commitSearch_1_3_2();
  void commitSearch_1_3_3();
  void commitSearch_1_3_4();
  void currentQuery_1_4_1();
  void currentQuery_1_4_2();
  void currentQuery_1_4_3();
  void currentQuery_1_4_4();
  void currentQuery_1_4_5();
  void currentQuery_1_4_7();
  void isEligibleForRequery_1_5_1();
  void isEligibleForRequery_1_5_2();
  void isEligibleForRequery_1_5_3();
  void queryArea_1_6_1();
  void queryArea_1_6_2();
  void queryArea_1_6_3();
  void queryArea_1_6_4();
  //void queryArea_1_6_5(); //skipped: modify geoview
  void queryCenter_1_7_1();
  void queryCenter_1_7_2();
  void searchResultMode_1_9_1();
  void searchResultMode_1_9_2();
  void searchResultMode_1_9_3();
  void searchResultMode_1_9_4();
  void searchResultMode_1_9_5();

signals:
  void waitThis();

private:
  Esri::ArcGISRuntime::LocatorTask* m_locatorTask{nullptr};
  Esri::ArcGISRuntime::Toolkit::LocatorSearchSource* m_locatorSource{nullptr};
  Esri::ArcGISRuntime::Toolkit::SearchViewController* controller{nullptr};

  std::chrono::system_clock::time_point start;

  void setDataTests();
};

#endif // TST_SEARCHVIEWFUNCTEST_H
