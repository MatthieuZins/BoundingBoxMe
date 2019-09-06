#ifndef VTKBOUNDINGBOXMANIPULATORWIDGET_H
#define VTKBOUNDINGBOXMANIPULATORWIDGET_H

#include "vtkInteractionWidgetsModule.h"
#include <vtkBoxWidget.h>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

class VTKINTERACTIONWIDGETS_EXPORT vtkBoundingBoxManipulatorWidget : public vtkBoxWidget
{
public:

  static vtkBoundingBoxManipulatorWidget *New();
  vtkTypeMacro(vtkBoundingBoxManipulatorWidget, vtkBoxWidget);


  void PlaceWidget();

  virtual void GetTransform(vtkTransform *t);

  vtkMatrix4x4* getPreviousUserTransform() {
    return prevUserTransform;
  }

protected:
  vtkBoundingBoxManipulatorWidget();
  ~vtkBoundingBoxManipulatorWidget() = default;
  double InitialCenter[3] = {0, 0, 0};
  double InitialScale[3] = {1.0, 1.0, 1.0};
  vtkSmartPointer<vtkMatrix4x4> prevUserTransform = nullptr;
  vtkSmartPointer<vtkMatrix4x4> initialOrientation = nullptr;
};

#endif // VTKBOUNDINGBOXMANIPULATORWIDGET_H
