#include "sideViewsInteractorStyle.h"

#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>
#include <vtkSmartPointer.h>
#include <QDebug>
#include "MainWindow.h"

vtkStandardNewMacro(SideViewsInteractorStyle);


void SideViewsInteractorStyle::OnKeyPress()
{
  // Get the keypress
  vtkRenderWindowInteractor *rwi = this->Interactor;
  std::string key = rwi->GetKeySym();

  if (key == "a" || key == "A" || key == "escape")
  {
    qInfo() << "Unselect all";
    m_mainWindowPtr->disableBoxWidget();
  }
  else if (key == "Delete")
  {
    qInfo() << "Remove bounding box";
    m_mainWindowPtr->deleteBoundingBox();
  }
  else if (key == "q" || key == "Q")
  {
    qInfo() << "Create new static bounding box";
    m_mainWindowPtr->createNewBoundingBox(BoundingBox::State::STATIC);
  }
  else if (key == "d" || key == "D")
  {
    qInfo() << "Create new dynamic bounding box";
    m_mainWindowPtr->createNewBoundingBox(BoundingBox::State::DYNAMIC);
  }

  // Forward events
  vtkInteractorStyleImage::OnKeyPress();
  m_mainWindowPtr->forceRender();
}

void SideViewsInteractorStyle::OnLeftButtonDown()
{
  this->m_nbClicks++;
  int pickPosition[2];
  this->GetInteractor()->GetEventPosition(pickPosition);

  int xdist = pickPosition[0] - this->m_previousPosition[0];
  int ydist = pickPosition[1] - this->m_previousPosition[1];

  this->m_previousPosition[0] = pickPosition[0];
  this->m_previousPosition[1] = pickPosition[1];

  int moveDistance = (xdist * xdist + ydist * ydist);

  // Reset numClicks - If mouse moved further than resetPixelDistance
  if(moveDistance > this->m_maxSqDistFoDoubleClick)
  {
    this->m_nbClicks = 1;
    // Test if we pick a bounding box
    auto picker = vtkSmartPointer<vtkPropPicker>::New();
    picker->Pick(pickPosition[0], pickPosition[1], 0, m_renderer);
    m_mainWindowPtr->disableBoxWidget();
    if (picker->GetActor())
    {
      m_mainWindowPtr->selectBoundingBox(picker->GetActor());
    }
  }

  if(this->m_nbClicks == 2)
  {
    this->m_nbClicks = 0;
    this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
                                        this->Interactor->GetEventPosition()[1],
                                        0,  // always zero.
                                        m_renderer);
    double picked[3];
    this->Interactor->GetPicker()->GetPickPosition(picked);
    m_mainWindowPtr->setFocalPoint(picked);
  }



  // Forward events
  vtkInteractorStyleImage::OnLeftButtonDown();
}
