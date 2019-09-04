#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPointPicker.h>

#include <vtkActor.h>
#include <vtkSphereSource.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkBoxWidget.h>
#include <vtkProp3D.h>
#include <vtkTransform.h>
#include <vtkCommand.h>
#include <vtkPLYReader.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRendererCollection.h>
#include <vtkObjectFactory.h>
#include <vtkPropPicker.h>
#include <QDebug>


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

// Define interaction style
class MouseInteractorStylePP : public vtkInteractorStyleTrackballCamera
{
  public:
    static MouseInteractorStylePP* New();
    vtkTypeMacro(MouseInteractorStylePP, vtkInteractorStyleTrackballCamera);

    virtual void OnLeftButtonDown()
    {
      std::cout << "Picking pixel: " << this->Interactor->GetEventPosition()[0] << " " << this->Interactor->GetEventPosition()[1] << std::endl;
      this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
                         this->Interactor->GetEventPosition()[1],
                         0,  // always zero.
                         this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
      double picked[3];
      this->Interactor->GetPicker()->GetPickPosition(picked);
      std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;
      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

};
vtkStandardNewMacro(MouseInteractorStylePP);

// Handle mouse events
class MouseInteractorStyle2 : public vtkInteractorStyleTrackballCamera
{
  public:
    static MouseInteractorStyle2* New();
    vtkTypeMacro(MouseInteractorStyle2, vtkInteractorStyleTrackballCamera);

    void setMainWindow(MainWindow* ptr) {
      m_mainwindowPtr = ptr;
    }

    virtual void OnLeftButtonDown()
    {
      int* clickPos = this->GetInteractor()->GetEventPosition();

      // Pick from this location.
      vtkSmartPointer<vtkPropPicker>  picker =
        vtkSmartPointer<vtkPropPicker>::New();
      picker->Pick(clickPos[0], clickPos[1], 0, this->GetDefaultRenderer());

      double* pos = picker->GetPickPosition();
      std::cout << "Pick position (world coordinates) is: "
                << pos[0] << " " << pos[1]
                << " " << pos[2] << std::endl;

      std::cout << "Picked actor: " << picker->GetActor() << std::endl;
//      //Create a sphere
//      vtkSmartPointer<vtkSphereSource> sphereSource =
//        vtkSmartPointer<vtkSphereSource>::New();
//      sphereSource->SetCenter(pos[0], pos[1], pos[2]);
//      sphereSource->SetRadius(0.1);

//      //Create a mapper and actor
//      vtkSmartPointer<vtkPolyDataMapper> mapper =
//        vtkSmartPointer<vtkPolyDataMapper>::New();
//      mapper->SetInputConnection(sphereSource->GetOutputPort());

//      vtkSmartPointer<vtkActor> actor =
//        vtkSmartPointer<vtkActor>::New();
//      actor->SetMapper(mapper);


      //this->GetInteractor()->GetRenderWindow()->GetRenderers()->GetDefaultRenderer()->AddActor(actor);
//      this->GetDefaultRenderer()->AddActor(actor);

      if (m_mainwindowPtr)
        m_mainwindowPtr->selectBoundingBox(picker->GetActor());

      // Forward events
      vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
    }

  private:
    MainWindow* m_mainwindowPtr = nullptr;

};
vtkStandardNewMacro(MouseInteractorStyle2);



MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  m_classesManager(ClassesManager::getInstance()),
  m_lidarFramesManager(LidarFrameManager::getInstance()),
  m_timeStepsManager(TimeStepsManager::getInstance(0)),
  m_boundingBoxManager(BoundingBoxManager::getInstance()),
  m_renderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
  m_renderer(vtkSmartPointer<vtkRenderer>::New()),
  m_axes(vtkSmartPointer<vtkAxesActor>::New()),
  m_widget(vtkSmartPointer<vtkOrientationMarkerWidget>::New()),
  m_boxWidget(vtkSmartPointer<vtkBoxWidget>::New()),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  vtkNew<vtkNamedColors> colors;
  m_boundingBoxManager.initializeClassesToHandle(m_classesManager);

//  m_boundingBoxManager.appendBoundingBox(0, "car", Eigen::Translation3d(1, 1, 0) * Eigen::Isometry3d::Identity(), Eigen::Vector3d::Ones(), 0);
//  m_boundingBoxManager.appendBoundingBox(1, "car", Eigen::Translation3d(-1, -2, 0) * Eigen::Isometry3d::Identity(), Eigen::Vector3d::Ones(), 0);

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
    auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    auto vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
    auto actor = vtkSmartPointer<vtkActor>::New();
    m_pcMappers.emplace_back(mapper);
    m_pcVertexGlyphFilters.emplace_back(vertexFilter);
    m_pcActors.emplace_back(actor);

    vertexFilter->SetInputData(pts);
    mapper->SetInputConnection(vertexFilter->GetOutputPort());
    actor->SetMapper(mapper);
    actor->GetProperty()->SetPointSize(3);
  }


//  vtkSmartPointer<vtkCubeSource> sphereSource = vtkSmartPointer<vtkCubeSource>::New();
//  sphereSource->Update();
//  vtkSmartPointer<vtkPolyDataMapper> sphereMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
//  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
//  vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();
//  sphereActor->SetMapper(sphereMapper);
//  sphereActor->GetProperty()->SetColor(colors->GetColor4d("OrangeRed").GetData());
//  sphereActor->GetProperty()->SetRepresentationToWireframe();
//  m_renderer->AddActor(sphereActor);

//  auto* boxWidget = vtkBoxWidget::New();
//   boxWidget->SetInteractor( m_renderWindow->GetInteractor() );

//   boxWidget->SetProp3D( sphereActor );
//   boxWidget->SetPlaceFactor(1.0);
//   boxWidget->PlaceWidget();

  auto callback = vtkSmartPointer<vtkMyCallback>::New();
  m_boxWidget->SetInteractor(m_renderWindow->GetInteractor());
  m_boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
  m_boxWidget->SetPlaceFactor(1.0);
  m_boxWidget->Off();


//  auto pointPicker = vtkSmartPointer<vtkPointPicker>::New();
//  auto style = vtkSmartPointer<MouseInteractorStylePP>::New();
//  style->SetDefaultRenderer(m_renderer);
//  ui->qvtkWidget->GetInteractor()->SetInteractorStyle(style);

  auto style2 = vtkSmartPointer<MouseInteractorStyle2>::New();
  style2->SetDefaultRenderer(m_renderer);
  style2->setMainWindow(this);
  ui->qvtkWidget->GetInteractor()->SetInteractorStyle(style2);

  addBoundingNewBox(Eigen::Translation3d(1, 1, 0));
  addBoundingNewBox(Eigen::Translation3d(2, -1, 0));
  addBoundingNewBox(Eigen::Translation3d(-2, 2, 0));
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::update()
{
  auto [first_frame, last_frame] = m_timeStepsManager.getTimeStepsInterval();
  qDebug() << "interval = " << first_frame << " -> " << last_frame;
  for (unsigned int i = 0; i < m_lidarFramesManager.getNbFrames(); ++i)
  {
    if (i >= first_frame && i <= last_frame)
      m_renderer->AddActor(m_pcActors[i]);
    else
      m_renderer->RemoveActor(m_pcActors[i]);
  }

  for (auto& bbActor : m_bbActors)
  {
    m_renderer->AddActor(bbActor);
  }

  m_renderer->Modified();
  m_renderer->Render();
}

void MainWindow::initialize()
{
  m_classesManager.loadFromYaml("../classes.yaml");
}

void MainWindow::displayLog(const QString& msg)
{
  ui->textBrowser_logger->append(msg);
}

void MainWindow::addBoundingNewBox(const Eigen::Translation3d& temp_transl)
{
  qInfo() << "Add Bounding Box";
  std::cout << "===============++> add bb" << std::endl;
  auto bb = m_boundingBoxManager.appendBoundingBox(m_bbSources.size(), "car", temp_transl * Eigen::Isometry3d::Identity(), Eigen::Vector3d::Ones(), 0);
  bb->addPresenceInFrame( temp_transl * Eigen::Isometry3d::Identity(), 0);

  auto source = vtkSmartPointer<vtkCubeSource>::New();
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  auto actor = vtkSmartPointer<vtkActor>::New();
  m_bbSources.emplace_back(source);
  m_bbMappers.emplace_back(mapper);
  m_bbActors.emplace_back(actor);

  vtkNew<vtkNamedColors> colors;

  mapper->SetInputConnection(source->GetOutputPort());
  actor->SetMapper(mapper);
  actor->SetPosition(bb->getCenter(0).data());
  actor->GetProperty()->SetColor(colors->GetColor4d("OrangeRed").GetData());
  actor->GetProperty()->SetRepresentationToSurface();
  actor->GetProperty()->SetOpacity(0.2);
  m_renderer->AddActor(actor);
}

void MainWindow::selectBoundingBox(vtkActor *bbActor)
{
  int idx = -1;
  for (unsigned int i = 0; i < m_bbActors.size(); ++i)
  {
    if (bbActor == m_bbActors[i])
    {
      idx = i;
      break;
    }
  }
  if (idx != -1)
  {
    qInfo() << "Found BB " << idx;
    m_boxWidget->SetProp3D(bbActor);
    m_boxWidget->PlaceWidget();
    m_boxWidget->On();
  }
  else
  {
    qWarning() << "Bounding Boxx not found";
  }
}


