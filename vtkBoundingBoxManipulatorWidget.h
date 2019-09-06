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

  vtkGetVector3Macro(actorCenter, double);
  vtkGetVector3Macro(actorOrigin, double);
  vtkGetVector3Macro(actorOrientation, double);
  vtkGetVector3Macro(actorScale, double);
  vtkSmartPointer<vtkMatrix4x4> userMatrix = nullptr;

protected:
  vtkBoundingBoxManipulatorWidget();
  ~vtkBoundingBoxManipulatorWidget() = default;
  double InitialCenter[3] = {0, 0, 0};
  double InitialScale[3] = {1.0, 1.0, 1.0};

  double actorCenter[3] = {0, 0, 0};
  double actorOrigin[3] = {0, 0, 0};
  double actorOrientation[3] = {0, 0, 0};
  double actorScale[3] = {0, 0, 0};

  vtkSmartPointer<vtkMatrix4x4> prevUserTransform = nullptr;
  vtkSmartPointer<vtkMatrix4x4> initialOrientation = nullptr;
};

#endif // VTKBOUNDINGBOXMANIPULATORWIDGET_H
