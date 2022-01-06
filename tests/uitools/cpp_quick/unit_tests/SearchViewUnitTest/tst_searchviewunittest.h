#ifndef TST_SEARCHVIEWUNITTEST_H
#define TST_SEARCHVIEWUNITTEST_H
#include <QtTest>

class SearchViewUnitTest : public QObject
{
  Q_OBJECT

public:
  SearchViewUnitTest();
  ~SearchViewUnitTest();

private slots:
  void initTestCase();
  void cleanupTestCase();
  void test_case1();
};

#endif // TST_SEARCHVIEWUNITTEST_H
