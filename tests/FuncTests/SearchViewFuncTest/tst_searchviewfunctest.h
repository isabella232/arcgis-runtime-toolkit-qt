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

  const QString magersBooksellers{QStringLiteral("Magers & Quinn Booksellers")};

public:
  SearchViewFuncTest() = default;
  ~SearchViewFuncTest() = default;

private slots:
  void initTestCase();
  void init(); // clearing preset things from previous tests

  void acceptSuggestion_1_1_1();
  void activeSource_1_2_1();

  void activeSource_1_2_2();

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
