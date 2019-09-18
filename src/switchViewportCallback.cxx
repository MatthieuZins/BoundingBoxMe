#include "switchViewportCallback.h"

#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include "mainViewInteractorStyle.h"
#include "sideViewsInteractorStyle.h"


void SwitchViewportCallback::Execute(vtkObject *, unsigned long event, void *)
{
  int lastPos[2];
  m_interactor->GetLastEventPosition(lastPos);
  int currPos[2];
  m_interactor->GetEventPosition(currPos);

  if (event == vtkCommand::MouseMoveEvent)
  {
    vtkRenderer *renderer = m_interactor->FindPokedRenderer(currPos[0], currPos[1]);
    if (renderer == m_renderer3d)
    {
      m_interactor->SetInteractorStyle(m_style3d);
    }
    else if (renderer == m_renderer2d_2)
    {
      m_interactor->SetInteractorStyle(m_style2d_2);
      m_style2d_2->SetDefaultRenderer(renderer);
    }
    else if (renderer == m_renderer2d_3)
    {
      m_interactor->SetInteractorStyle(m_style2d_3);
      m_style2d_3->SetDefaultRenderer(renderer);
    }
    else if (renderer == m_renderer2d_4)
    {
      m_interactor->SetInteractorStyle(m_style2d_4);
      m_style2d_4->SetDefaultRenderer(renderer);
    }

    vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(m_interactor->GetInteractorStyle());
    if (style)
    {
      style->OnMouseMove();
    }
  }
}
