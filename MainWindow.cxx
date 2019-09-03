#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>

#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkBoxWidget.h>
#include <vtkProp3D.h>
#include <vtkTransform.h>
#include <vtkCommand.h>
#include <vtkPLYReader.h>

class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback *New()
  {
    return new vtkMyCallback;
  }
  virtual void Execute( vtkObject *caller, unsigned long, void* )
  {
    // Here we use the vtkBoxWidget to transform the underlying coneActor
    // (by manipulating its transformation matrix).
    vtkSmartPointer<vtkTransform> t =
      vtkSmartPointer<vtkTransform>::New();
    vtkBoxWidget *widget = reinterpret_cast<vtkBoxWidget*>(caller);
    widget->GetTransform( t );
    widget->GetProp3D()->SetUserTransform( t );
  }
};

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent), m_lidarFramesManager(LidarFrameManager::getInstance()),
  m_timeStepsManager(TimeStepsManager::getInstance(0)),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  vtkNew<vtkNamedColors> colors;

  vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
  this->ui->qvtkWidget->SetRenderWindow(renderWindow);

  m_renderer->SetBackground(colors->GetColor3d("Grey").GetData());

  this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

  m_axes = vtkSmartPointer<vtkAxesActor>::New();
  m_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  m_widget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_widget->SetOrientationMarker(m_axes);
  m_widget->SetInteractor(this->ui->qvtkWidget->GetInteractor());
  m_widget->SetViewport(0.0, 0.0, 0.2, 0.2);
  m_widget->SetEnabled(1);
  m_widget->InteractiveOn();

  auto reader = vtkSmartPointer<vtkPLYReader>::New();
//  reader->SetFileName("/home/matthieu/dev/CalibrationDepthPose/data/pc_0000.ply");
//  reader->Update();


  m_timeStepsManager.setSize(6);
  m_timeStepsManager.setModeAll();

  for (int i = 0; i <6; ++i)
  {
    reader->SetFileName(std::string("/home/matthieu/dev/CalibrationDepthPose/data/pc_000" + std::to_string(i) + ".ply").c_str());
    reader->Update();
    m_lidarFramesManager.addFrame({i, reader->GetOutput()});
  }

  auto pointsToRender = m_lidarFramesManager.getFramesPoints();
  for (auto* pts : pointsToRender)
  {
    m_mappers.push_back(vtkSmartPointer<vtkPolyDataMapper>::New());
    m_vertexGlyphFilters.push_back(vtkSmartPointer<vtkVertexGlyphFilter>::New());
    m_actors.push_back(vtkSmartPointer<vtkActor>::New());

    m_vertexGlyphFilters.back()->SetInputData(pts);
    m_mappers.back()->SetInputConnection(m_vertexGlyphFilters.back()->GetOutputPort());
    m_actors.back()->SetMapper(m_mappers.back());
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::update()
{
  auto [first_frame, last_frame] = m_timeStepsManager.getTimeStepsInterval();
  std::cout << "interval = " << first_frame << " -> " << last_frame << std::endl;
  for (unsigned int i = 0; i < m_lidarFramesManager.getNbFrames(); ++i)
  {
    if (i >= first_frame && i <= last_frame)
      m_renderer->AddActor(m_actors[i]);
    else
      m_renderer->RemoveActor(m_actors[i]);
  }
  m_renderer->Modified();
  m_renderer->Render();
}
