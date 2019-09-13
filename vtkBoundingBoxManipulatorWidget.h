//=========================================================================
//
// Copyright 2019 Kitware, Inc.
// Author: Matthieu Zins
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef VTKBOUNDINGBOXMANIPULATORWIDGET_H
#define VTKBOUNDINGBOXMANIPULATORWIDGET_H

#include "vtkInteractionWidgetsModule.h"
#include <vtkBoxWidget.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>

class VTKINTERACTIONWIDGETS_EXPORT vtkBoundingBoxManipulatorWidget : public vtkBoxWidget
{
public:

  static vtkBoundingBoxManipulatorWidget *New();
  vtkTypeMacro(vtkBoundingBoxManipulatorWidget, vtkBoxWidget);


  // This method should be called when place the widget on an actor
  void PlaceWidget() override;

  // Override this function to do nothing
  // For this widget getPoseMatrix should be used instead
  virtual void GetTransform(vtkTransform *t) override {}

  void forceRepresentationToSurface();

  // This method returns the current pose of the widget (in world coordinates)
  vtkSmartPointer<vtkMatrix4x4> getPoseMatrix();

protected:
  vtkBoundingBoxManipulatorWidget() = default;
  ~vtkBoundingBoxManipulatorWidget() = default;
};


inline void vtkBoundingBoxManipulatorWidget::forceRepresentationToSurface()
{
  this->GetFaceProperty()->SetRepresentationToSurface();
}

#endif // VTKBOUNDINGBOXMANIPULATORWIDGET_H
