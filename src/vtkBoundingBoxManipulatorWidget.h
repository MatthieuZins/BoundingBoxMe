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
#include <vtkCommand.h>
#include <vtkProperty.h>
#include <vtkSmartPointer.h>


class MainWindow;
class vtkMatrix4x4;

/// This class is a callback which is called when the BBox manipulator has changed
class BBoxManipulatorCallback : public vtkCommand
{
public:
  static BBoxManipulatorCallback *New() {
    return new BBoxManipulatorCallback;
  }

  virtual void Execute( vtkObject *caller, unsigned long, void* );


  void setMainWindow(MainWindow* ptr) {
    m_mainwindowPtr = ptr;
  }

private:
  MainWindow* m_mainwindowPtr = nullptr;
};


/// This class is a specialization of the vtkBoxWidget adapted for our needs
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

  // This method returns the current pose of the widget (in world coordinates)
  vtkSmartPointer<vtkMatrix4x4> getPoseMatrix();

protected:
  vtkBoundingBoxManipulatorWidget() = default;
  ~vtkBoundingBoxManipulatorWidget() = default;
};


#endif // VTKBOUNDINGBOXMANIPULATORWIDGET_H
