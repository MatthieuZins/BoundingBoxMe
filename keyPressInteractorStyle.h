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

#ifndef KEYPRESSINTERACTORSTYLE_H
#define KEYPRESSINTERACTORSTYLE_H
#include <vtkInteractorStyleTrackballCamera.h>


class MainWindow;

// Define interaction style
class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
{
  public:
    static KeyPressInteractorStyle* New();
    vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

    KeyPressInteractorStyle() = default;
    ~KeyPressInteractorStyle() = default;

    virtual void OnKeyPress();

    virtual void OnLeftButtonDown();

    void setMainWindow(MainWindow* ptr) {
      m_mainWindowPtr = ptr;
    }

    void setRenderer(vtkRenderer* renderer) {
      m_renderer = renderer;
    }
private:
    MainWindow* m_mainWindowPtr = nullptr;
    vtkRenderer* m_renderer = nullptr;
    unsigned int m_nbClicks = 0;
    int m_previousPosition[2] = {0, 0};
    int m_maxSqDistFoDoubleClick = 25;
};


#endif // KEYPRESSINTERACTORSTYLE_H
