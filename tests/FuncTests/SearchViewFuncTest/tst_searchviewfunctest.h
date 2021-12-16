#ifndef TST_SEARCHVIEWFUNCTEST_H
#define TST_SEARCHVIEWFUNCTEST_H

#include <QtTest>

namespace Esri {
namespace ArcGISRuntime {
  class LocatorTask;
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
  void acceptSuggestion_1_1_1();
  void activeSource_1_2_1();
  void activeSource_1_2_2();

signals:
  void waitThis();

private:
  Esri::ArcGISRuntime::LocatorTask* m_locatorTask;
  Esri::ArcGISRuntime::Toolkit::LocatorSearchSource* m_locatorSource;
  Esri::ArcGISRuntime::Toolkit::SearchViewController* controller;
  std::chrono::system_clock::time_point start;
};

#endif // TST_SEARCHVIEWFUNCTEST_H
