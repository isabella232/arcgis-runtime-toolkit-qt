// add necessary includes here
#include "tst_searchviewfunctest.h"

#include "SearchViewController.h"

SearchViewFuncTest::SearchViewFuncTest()
{

}

SearchViewFuncTest::~SearchViewFuncTest()
{

}

void SearchViewFuncTest::test_case1()
{
  auto searchview = new Esri::ArcGISRuntime::Toolkit::SearchViewController();
  QVERIFY(searchview);
}

QTEST_MAIN(SearchViewFuncTest)
