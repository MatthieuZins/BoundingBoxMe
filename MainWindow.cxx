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
#include <unsupported/Eigen/EulerAngles>
#include <vtkMatrix4x4.h>
#include "keyPressInteractorStyle.h"

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
    vtkBoundingBoxManipulatorWidget *widget = reinterpret_cast<vtkBoundingBoxManipulatorWidget*>(caller);

    widget->GetTransform(0);  // this is just use to update the internal state of the widget

    auto* actorToModify = widget->GetProp3D();

    actorToModify->SetUserMatrix(widget->getPoseMatrixCopy());

  }

  void setMainWindow(MainWindow* ptr) {
    m_mainwindowPtr = ptr;
  }
private:
  MainWindow* m_mainwindowPtr = nullptr;
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
  m_axesWidget(vtkSmartPointer<vtkOrientationMarkerWidget>::New()),
  m_boxWidget(vtkSmartPointer<vtkBoundingBoxManipulatorWidget>::New()),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  vtkNew<vtkNamedColors> colors;

//  m_boundingBoxManager.appendBoundingBox(0, "car", Eigen::Translation3d(1, 1, 0) * Eigen::Isometry3d::Identity(), Eigen::Vector3d::Ones(), 0);
//  m_boundingBoxManager.appendBoundingBox(1, "car", Eigen::Translation3d(-1, -2, 0) * Eigen::Isometry3d::Identity(), Eigen::Vector3d::Ones(), 0);

  this->ui->qvtkWidget->SetRenderWindow(m_renderWindow);

  m_renderer->SetBackground(colors->GetColor3d("Grey").GetData());

  this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

  m_axesWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_axesWidget->SetOrientationMarker(m_axes);
  m_axesWidget->SetInteractor(this->ui->qvtkWidget->GetInteractor());
  m_axesWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
  m_axesWidget->SetEnabled(1);
  m_axesWidget->InteractiveOn();

  auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
//  reader->SetFileName("/home/matthieu/dev/CalibrationDepthPose/data/pc_0000.ply");
//  reader->Update();

  int NB = 501;

  m_timeStepsManager.initializeSize(NB);
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
  callback->setMainWindow(this);
  m_boxWidget->SetInteractor(m_renderWindow->GetInteractor());
  m_boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
  m_boxWidget->SetPlaceFactor(1.0);
  m_boxWidget->Off();


//  auto pointPicker = vtkSmartPointer<vtkPointPicker>::New();
//  auto style = vtkSmartPointer<MouseInteractorStylePP>::New();
//  style->SetDefaultRenderer(m_renderer);
//  ui->qvtkWidget->GetInteractor()->SetInteractorStyle(style);

//  auto style2 = vtkSmartPointer<MouseInteractorStyle2>::New();
//  style2->SetDefaultRenderer(m_renderer);
//  style2->setMainWindow(this);
//  ui->qvtkWidget->GetInteractor()->SetInteractorStyle(style2);

  auto style3 = vtkSmartPointer<KeyPressInteractorStyle>::New();
  style3->SetDefaultRenderer(m_renderer);
  style3->setMainWindow(this);
  ui->qvtkWidget->GetInteractor()->SetInteractorStyle(style3);


  addBoundingNewBox(Eigen::Translation3d(0, 0, 0));
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
  m_boundingBoxManager.initializeClassesToHandle(m_classesManager);
  ui->widget_BB_Information->updateAvailableClasses(m_classesManager.getAvailableClasses());
}

void MainWindow::displayLog(const QString& msg)
{
  ui->textBrowser_logger->append(msg);
}

void MainWindow::addBoundingNewBox(const Eigen::Translation3d& temp_transl)
{
  qInfo() << "Add Bounding Box";
  std::cout << "===============++> add bb" << std::endl;
  auto bb = m_boundingBoxManager.appendBoundingBox(m_bbSources.size(), "car", temp_transl * Eigen::Isometry3d::Identity() * Eigen::EulerAnglesXYZd(10, 20, 30), Eigen::Vector3d::Ones(), 0);
  bb->addPresenceInFrame( temp_transl * Eigen::Isometry3d::Identity() * Eigen::EulerAnglesXYZd(10, 20, 30), 0);

  auto source = vtkSmartPointer<vtkCubeSource>::New();
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  auto actor = vtkSmartPointer<vtkActor>::New();
  m_bbSources.emplace_back(source);
  m_bbMappers.emplace_back(mapper);
  m_bbActors.emplace_back(actor);

  vtkNew<vtkNamedColors> colors;

  mapper->SetInputConnection(source->GetOutputPort());
  actor->SetMapper(mapper);
//  actor->SetPosition(bb->getCenter(0).data());
//  actor->SetOrigin(bb->getCenter(0).data());
//  actor->SetOrientation(10, 20, 30);
  actor->SetUserMatrix(bb->getPoseVtkMatrix(0));
  actor->ComputeMatrix();
  actor->GetProperty()->SetColor(colors->GetColor4d("OrangeRed").GetData());
  actor->GetProperty()->SetRepresentationToSurface();
  actor->GetProperty()->SetOpacity(0.2);
  m_renderer->AddActor(actor);
}

int MainWindow::findBoundingBoxFromActor(vtkProp3D* actor)
{
  int idx = -1;
  for (unsigned int i = 0; i < m_bbActors.size(); ++i)
  {
    if (actor == static_cast<vtkProp3D*>(m_bbActors[i]))
    {
      idx = i;
      break;
    }
  }
  return idx;
}

void MainWindow::disableBoxWidget()
{
  std::cout << "====== disable box widget =======\n";
  // we need to updat the position and orientation fo the actor of the bb with its user transform
  auto* actor = m_boxWidget->GetProp3D();

//  auto* userT = actor->GetUserTransform();
//  if (userT)
//  {
//    auto* matrix = userT->GetMatrix();
//    if (matrix)
//    {
//      std::cout << "matrix = \n" << *matrix << std::endl;
//      Eigen::Vector3d pos = Eigen::Map<Eigen::Vector3d>(actor->GetPosition());
//      Eigen::Vector3d rot = Eigen::Map<Eigen::Vector3d>(actor->GetOrientation());
//      Eigen::EulerAnglesZXYd eulerAngles(rot[2], rot[0], rot[1]);
//      Eigen::EulerAnglesYXZd eulerAngles2(rot[1], rot[0], rot[2]);
//      Eigen::Vector3d origin = Eigen::Map<Eigen::Vector3d>(actor->GetOrigin());
//  //    Eigen::Isometry3d T = Eigen::Translation3d(position) * Eigen::Translation3d(origin) * eulerAngles2 * Eigen::Translation3d(-origin);
//      std::cout << "origin = " << origin.transpose() << std::endl;
//    }
//  }

  m_boxWidget->Off();
}

void MainWindow::selectBoundingBox(vtkActor *bbActor)
{
  int idx = findBoundingBoxFromActor(bbActor);
  if (idx != -1)
  {
    qInfo() << "Select bounding box" << idx;
    m_boxWidget->SetProp3D(bbActor);
//    auto* m = bbActor->GetUserTransform();
//    std::cout << "user matrix at selection is \n";
//    if (m)
//    {
//      std::cout << *m << "\n";
//    }
//    else
//    {
//      std::cout << "no\n";
//    }

    m_boxWidget->PlaceWidget();
//    if (bbActor->GetUserTransform())
//    {
//      auto * ma = bbActor->GetUserTransform()->GetMatrix();
////      ma->SetElement(0, 3, 0);
////      ma->SetElement(1, 3, 0);
////      ma->SetElement(2, 3, 0);
////      ma->Identity();
//    auto mb = vtkSmartPointer<vtkMatrix4x4>::New();
//    mb->DeepCopy(ma);
//    mb->Identity();
//      vtkSmartPointer<vtkTransform> t = vtkSmartPointer<vtkTransform>::New();
//      t->SetMatrix(mb);
//      m_boxWidget->SetTransform(t);

//    }
    m_boxWidget->On();
    this->ui->widget_BB_Information->updateInformation(m_boundingBoxManager.getBoundingBoxFromIndex(idx));
  }
}


