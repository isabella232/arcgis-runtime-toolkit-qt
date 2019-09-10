/*******************************************************************************
 *  Copyright 2012-2019 Esri
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

#ifndef ArcGISArView_H
#define ArcGISArView_H

#include "ArcGISArViewInterface.h"
#include "Camera.h"
#include "SceneQuickView.h"

namespace Esri {
namespace ArcGISRuntime {
class TransformationMatrixCameraController;
namespace Toolkit {

class ArcGISArView : public ArcGISArViewInterface
{
  Q_OBJECT

  Q_PROPERTY(Camera originCamera READ originCamera WRITE setOriginCamera NOTIFY originCameraChanged)
  Q_PROPERTY(SceneQuickView* sceneView READ sceneView WRITE setSceneView NOTIFY sceneViewChanged)

public:
  explicit ArcGISArView(QQuickItem* parent = nullptr);
  explicit ArcGISArView(bool renderVideoFeed, bool tryUsingArKit, QQuickItem* parent = nullptr);
  ~ArcGISArView() override;

  // properties
  Camera originCamera() const;
  void setOriginCamera(const Camera& originCamera);

  SceneQuickView* sceneView() const;
  void setSceneView(SceneQuickView* sceneView);

  // methods
  Q_INVOKABLE void setInitialTransformation(const QPoint& screenPoint);
  Q_INVOKABLE Point screenToLocation(const Point& screenPoint) const;

signals:
  void originCameraChanged();
  void sceneViewChanged();

public: // internals, used by AR wrappers
  void setTransformationMatrixInternal(double quaternionX, double quaternionY, double quaternionZ, double quaternionW,
                                       double translationX, double translationY, double translationZ) override;

  void setFieldOfViewInternal(double xFocalLength, double yFocalLength,
                              double xPrincipal, double yPrincipal,
                              double xImageSize, double yImageSize) override;

  void renderFrameInternal() override;

protected:
  void setTranslationFactorInternal(double translationFactor) override;
  void setLocationInternal(double latitude, double longitude, double altitude) override;
  void setHeadingInternal(double heading) override;
  void resetTrackingInternal() override;

private:
  int m_timerId = 0;
  SceneQuickView* m_sceneView = nullptr;

  // The `AGSTransformationMatrixCameraController` used to control the Scene.
  TransformationMatrixCameraController* m_tmcc = nullptr;

  // The initial transformation used for a table top experience. Defaults to the Identity Matrix.
  TransformationMatrix* m_initialTransformation = nullptr;

  // The viewpoint camera used to set the initial view of the sceneView instead of the device's GPS
  // location via the location data source.  You can use Key-Value Observing to track changes to the origin camera.
  Camera m_originCamera;
};

} // Toolkit namespace
} // ArcGISRuntime namespace
} // Esri namespace

#endif // ArcGISArView_H
