#ifndef TST_SEARCHVIEWFUNCTEST_H
#define TST_SEARCHVIEWFUNCTEST_H

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

signals:
  void waitThis();

private:
  Esri::ArcGISRuntime::LocatorTask* m_locatorTask{nullptr};
  Esri::ArcGISRuntime::Toolkit::LocatorSearchSource* m_locatorSource{nullptr};
  Esri::ArcGISRuntime::Toolkit::SearchViewController* controller{nullptr};
  Esri::ArcGISRuntime::MapQuickView* m_mapView{nullptr};

  std::chrono::system_clock::time_point start;
};

#endif // TST_SEARCHVIEWFUNCTEST_H
