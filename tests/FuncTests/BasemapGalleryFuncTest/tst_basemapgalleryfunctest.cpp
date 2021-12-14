#include "tst_basemapgalleryfunctest.h"

// add necessary includes here
#include "BasemapGalleryController.h"
BasemapGalleryFuncTest::BasemapGalleryFuncTest()
{

}

BasemapGalleryFuncTest::~BasemapGalleryFuncTest()
{

}

void BasemapGalleryFuncTest::test_case1()
{
  auto basemapgallery = new Esri::ArcGISRuntime::Toolkit::BasemapGalleryController();
}

QTEST_MAIN(BasemapGalleryFuncTest)
