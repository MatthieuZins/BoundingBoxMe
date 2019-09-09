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

  std::cout << "key = " << key << std::endl;
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
  int* clickPos = this->GetInteractor()->GetEventPosition();

  // Pick from this location.
  auto picker = vtkSmartPointer<vtkPropPicker>::New();
  picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

  double* pos = picker->GetPickPosition();
  std::cout << "Pick position (world coordinates) is: "
            << pos[0] << " " << pos[1]
            << " " << pos[2] << std::endl;

  std::cout << "Picked actor: " << picker->GetActor() << std::endl;

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


  // Forward events
  vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}
