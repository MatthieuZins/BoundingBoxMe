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

#include "keyPressInteractorStyle.h"

#include <vtkObjectFactory.h>
#include <vtkRendererCollection.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPropPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartPointer.h>
#include <QDebug>

#include "MainWindow.h"


vtkStandardNewMacro(KeyPressInteractorStyle);


void KeyPressInteractorStyle::OnKeyPress()
{
  // Get the keypress
  vtkRenderWindowInteractor *rwi = this->Interactor;
  std::string key = rwi->GetKeySym();

  if (key == "a" || key == "A")
  {
    qInfo() << "Unselect all";
    m_mainWindowPtr->disableBoxWidget();
  }
  else if (key == "Delete")
  {
    qInfo() << "Remove bounding box";
    m_mainWindowPtr->deleteBoundingBox();
  }
  else if (key == "n")
  {
    qInfo() << "Create new bounding box";
    m_mainWindowPtr->createNewBoundingBox();
  }

  // Forward events
  vtkInteractorStyleTrackballCamera::OnKeyPress();
  m_mainWindowPtr->forceRender();
}

void KeyPressInteractorStyle::OnLeftButtonDown()
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
    picker->Pick(pickPosition[0], pickPosition[1], 0, this->GetDefaultRenderer());
    if (picker->GetActor())
    {
      if (m_mainWindowPtr)
        m_mainWindowPtr->selectBoundingBox(picker->GetActor());
    }
    else
    {
      qInfo() << "Unselect all";
      m_mainWindowPtr->disableBoxWidget();
    }
  }

  if(this->m_nbClicks == 2)
  {
    this->m_nbClicks = 0;
    this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
                                        this->Interactor->GetEventPosition()[1],
                                        0,  // always zero.
                                        this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
    double picked[3];
    this->Interactor->GetPicker()->GetPickPosition(picked);
    vtkCamera* camera = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera();
    camera->SetFocalPoint(picked);
  }



  // Forward events
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}
