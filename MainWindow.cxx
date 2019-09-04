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
#include <vtkXMLPolyDataReader.h>

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
  QMainWindow(parent),
  m_classesManager(ClassesManager::getInstance()),
  m_lidarFramesManager(LidarFrameManager::getInstance()),
  m_timeStepsManager(TimeStepsManager::getInstance(0)),
  m_renderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
  m_renderer(vtkSmartPointer<vtkRenderer>::New()),
  m_axes(vtkSmartPointer<vtkAxesActor>::New()),
  m_widget(vtkSmartPointer<vtkOrientationMarkerWidget>::New()),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  vtkNew<vtkNamedColors> colors;

  m_classesManager.loadFromYaml("../classes.yaml");

  this->ui->qvtkWidget->SetRenderWindow(m_renderWindow);

  m_renderer->SetBackground(colors->GetColor3d("Grey").GetData());

  this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

  m_widget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_widget->SetOrientationMarker(m_axes);
  m_widget->SetInteractor(this->ui->qvtkWidget->GetInteractor());
  m_widget->SetViewport(0.0, 0.0, 0.2, 0.2);
  m_widget->SetEnabled(1);
  m_widget->InteractiveOn();

  auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
//  reader->SetFileName("/home/matthieu/dev/CalibrationDepthPose/data/pc_0000.ply");
//  reader->Update();

  int NB = 501;

  m_timeStepsManager.setSize(NB);
  m_timeStepsManager.setModeAll();

  for (int i = 0; i < NB; ++i)
  {
    reader->SetFileName(std::string("../data/frame_" + std::to_string(i) + ".vtp").c_str());
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
