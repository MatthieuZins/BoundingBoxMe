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

  virtual void GetTransform(vtkTransform*);

  vtkSmartPointer<vtkMatrix4x4> getPoseMatrixCopy() const {
    vtkSmartPointer<vtkMatrix4x4> posecopy = vtkSmartPointer<vtkMatrix4x4>::New();
    posecopy->DeepCopy(userMatrix);
    return posecopy;
  }


protected:
  vtkBoundingBoxManipulatorWidget();
  ~vtkBoundingBoxManipulatorWidget() = default;

  vtkSmartPointer<vtkMatrix4x4> userMatrix = nullptr;


};

#endif // VTKBOUNDINGBOXMANIPULATORWIDGET_H
