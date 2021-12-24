import QtQuick 2.0
import QtTest 1.0

import Esri.ArcGISRuntime 100.13
import Esri.ArcGISRuntime.Toolkit 100.13

TestCase {
    name: "BasemapGalleryUnitTest"

    Basemap{

    }

    function initTestCase() {
    }

    function cleanupTestCase() {
    }

    function test_case1() {
        compare(1 + 1, 2, "sanity check");
        verify(true);
    }
}
