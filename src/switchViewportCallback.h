#ifndef SWITCHVIEWPORTCALLBACK_H
#define SWITCHVIEWPORTCALLBACK_H

#include <vtkCommand.h>
#include <vtkSmartPointer.h>

#include "mainViewInteractorStyle.h"
#include "sideViewsInteractorStyle.h"


class vtkRenderer;
class vtkRenderWindowInteractor;


/// This class is a callback to automatically change the active viewport when a mouse event happens
class SwitchViewportCallback : public vtkCommand
{
public:

  static SwitchViewportCallback *New() {
    return new SwitchViewportCallback;
  }

  void SetInteractor(vtkRenderWindowInteractor *interactor);

  vtkRenderWindowInteractor *GetInteractor();

  void Set3DInteractorStyle(vtkSmartPointer<MainViewInteractorStyle> m);

  void Set2DInteractorStyle(vtkSmartPointer<SideViewsInteractorStyle> style2,
                            vtkSmartPointer<SideViewsInteractorStyle> style3,
                            vtkSmartPointer<SideViewsInteractorStyle> style4);

  void setRenderers(vtkRenderer* renderer3d, vtkRenderer* renderer2d_2,
                    vtkRenderer* renderer2d_3,vtkRenderer* renderer2d_4);

  virtual void Execute(vtkObject *, unsigned long event, void *);

private:
  SwitchViewportCallback() = default;

  vtkRenderWindowInteractor *m_interactor = nullptr;

  // pointer to 3D interactor style
  vtkSmartPointer<MainViewInteractorStyle> m_style3d = nullptr;

  // pointer to 2d interactor styles
  vtkSmartPointer<SideViewsInteractorStyle> m_style2d_2 = nullptr;
  vtkSmartPointer<SideViewsInteractorStyle> m_style2d_3 = nullptr;
  vtkSmartPointer<SideViewsInteractorStyle> m_style2d_4 = nullptr;

 // pointer to image renderers
  vtkRenderer *m_renderer3d = nullptr;
  vtkRenderer *m_renderer2d_2 = nullptr;
  vtkRenderer *m_renderer2d_3 = nullptr;
  vtkRenderer *m_renderer2d_4 = nullptr;
};



inline void SwitchViewportCallback::SetInteractor(vtkRenderWindowInteractor *interactor)
{
  m_interactor = interactor;
}

inline vtkRenderWindowInteractor *SwitchViewportCallback::GetInteractor()
{
  return this->m_interactor;
}

inline void SwitchViewportCallback::Set3DInteractorStyle(vtkSmartPointer<MainViewInteractorStyle> m)
{
  this-> m_style3d = m;
}


inline void SwitchViewportCallback::Set2DInteractorStyle(vtkSmartPointer<SideViewsInteractorStyle> style2,
                                                         vtkSmartPointer<SideViewsInteractorStyle> style3,
                                                         vtkSmartPointer<SideViewsInteractorStyle> style4)
{
  m_style2d_2 = style2;
  m_style2d_3 = style3;
  m_style2d_4 = style4;
}


inline void SwitchViewportCallback::setRenderers(vtkRenderer *renderer3d,
                                                 vtkRenderer *renderer2d_2,
                                                 vtkRenderer *renderer2d_3,
                                                 vtkRenderer *renderer2d_4)
{
  m_renderer3d = renderer3d;
  m_renderer2d_2 = renderer2d_2;
  m_renderer2d_3 = renderer2d_3;
  m_renderer2d_4 = renderer2d_4;
}

#endif // SWITCHVIEWPORTCALLBACK_H
