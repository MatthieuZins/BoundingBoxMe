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
