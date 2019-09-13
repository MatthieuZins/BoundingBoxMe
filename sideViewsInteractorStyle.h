#ifndef SIDEVIEWSINTERACTORSTYLE_H
#define SIDEVIEWSINTERACTORSTYLE_H

#include <vtkInteractorStyleImage.h>

class MainWindow;

// Define interaction style
class SideViewsInteractorStyle : public vtkInteractorStyleImage
{
  public:
    static SideViewsInteractorStyle* New();
    vtkTypeMacro(SideViewsInteractorStyle, vtkInteractorStyleImage);

    SideViewsInteractorStyle() = default;
    ~SideViewsInteractorStyle() = default;

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


#endif // SIDEVIEWSINTERACTORSTYLE_H
