/*******************************************************************************
 *  Copyright 2012-2021 Esri
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ******************************************************************************/
#include <QDir>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickWindow>
#include <QQuickStyle>

//------------------------------------------------------------------------------

int
main(int argc, char* argv[])
{
  qDebug() << "Initializing application";

  QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QGuiApplication app(argc, argv);

  // Use of Esri location services, including basemaps and geocoding, requires
  // either an ArcGIS identity or an API key. For more information see
  // https://links.esri.com/arcgis-runtime-security-auth.

  // 1. ArcGIS identity: An ArcGIS named user account that is a member of an
  // organization in ArcGIS Online or ArcGIS Enterprise.

  // 2. API key: A permanent key that gives your application access to Esri
  // location services. Create a new API key or access existing API keys from
  // your ArcGIS for Developers dashboard
  // (https://links.esri.com/arcgis-api-keys).

  const QString apiKey =
    QString("");
  if (apiKey.isEmpty()) {
    qWarning() << "Use of Esri location services, including basemaps, requires "
                  "you to authenticate with an ArcGIS identity or set the API "
                  "Key property.";
  } else {
    QCoreApplication::instance()->setProperty("Esri.ArcGISRuntime.apiKey",
                                              apiKey);
  }

  // Production deployment of applications built with ArcGIS Runtime requires
  // you to license ArcGIS Runtime functionality. For more information see
  // https://links.esri.com/arcgis-runtime-license-and-deploy.

  // QCoreApplication::instance()->setProperty("Esri.ArcGISRuntime.license",
  // "licenseString");

  // This is how we make the style visible to our application!
  // We can now apply the style via the `qtquickcontrol2.conf` file.
  // See `README.md` for details.
  QQuickStyle::addStylePath("qrc:///esri.com/imports/");

  // Intialize application window
  QQmlApplicationEngine appEngine;
  appEngine.addImportPath("qrc:///esri.com/imports/");

#ifdef ARCGIS_RUNTIME_IMPORT_PATH_2
  appEngine.addImportPath(ARCGIS_RUNTIME_IMPORT_PATH_2);
#endif

  appEngine.load(QUrl("qrc:/qml/main.qml"));
  return app.exec();
}
