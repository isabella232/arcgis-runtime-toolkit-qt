/*******************************************************************************
 *  Copyright 2012-2018 Esri
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

#include "ArcGISArViewRenderer.h"
#include <QOpenGLFramebufferObjectFormat>

using namespace Esri::ArcGISRuntime;
using namespace Esri::ArcGISRuntime::Toolkit;

QOpenGLFramebufferObject* ArcGISArViewRenderer::createFramebufferObject(const QSize& size)
{
  QOpenGLFramebufferObjectFormat format;
  format.setAttachment(QOpenGLFramebufferObject::CombinedDepthStencil);
  return new QOpenGLFramebufferObject(size, format);
}

void ArcGISArViewRenderer::synchronize(QQuickFramebufferObject* item)
{
  if (!m_window && item)
    m_window = item->window();
}

void ArcGISArViewRenderer::render()
{
  if (!m_isInitialized)
  {
    m_isInitialized = true;
    m_arWrapper->init();
  }

  m_arWrapper->render();

  if (m_window)
    m_window->resetOpenGLState();
}

void ArcGISArViewRenderer::setArWrapper(ArWrapper* arWrapper)
{
  m_arWrapper = arWrapper;
}
