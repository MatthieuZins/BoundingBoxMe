#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkColorTransferFunction.h>
#include <vtkPointPicker.h>
#include <vtkColorSeries.h>
#include <vtkCamera.h>
#include <vtkActor.h>
#include <vtkActorCollection.h>
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
#include <vtkLookupTable.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRendererCollection.h>
#include <vtkObjectFactory.h>
#include <vtkPropPicker.h>
#include <QDebug>
#include <QFileDialog>
#include <unsupported/Eigen/EulerAngles>
#include <vtkMatrix4x4.h>
#include "keyPressInteractorStyle.h"
#include "vtkEigenUtils.h"
#include "datasetHelper.h"

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
    vtkBoundingBoxManipulatorWidget *widget = vtkBoundingBoxManipulatorWidget::SafeDownCast(caller);

    widget->GetTransform(0);  // this is just use to update the internal state of the widget

    auto* actorToModify = vtkActor::SafeDownCast(widget->GetProp3D());

    actorToModify->SetUserMatrix(widget->getPoseMatrix());
    if (m_mainwindowPtr)
    {
      auto index = m_mainwindowPtr->findBoundingBoxFromActor(actorToModify);
      if (index >= 0)
      {
        m_mainwindowPtr->editBoundingBox(index);
      }
    }
  }

  void setMainWindow(MainWindow* ptr) {
    m_mainwindowPtr = ptr;
  }
private:
  MainWindow* m_mainwindowPtr = nullptr;
};



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
  QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openLidarDataset()));
  QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveBoundingBoxDataset()));
  QObject::connect(ui->actionOpenBBox, SIGNAL(triggered()), this, SLOT(loadBoundingBoxDataset()));

  vtkNew<vtkNamedColors> colors;

//  m_boundingBoxManager.appendBoundingBox(0, "car", Eigen::Translation3d(1, 1, 0) * Eigen::Isometry3d::Identity(), Eigen::Vector3d::Ones(), 0);
//  m_boundingBoxManager.appendBoundingBox(1, "car", Eigen::Translation3d(-1, -2, 0) * Eigen::Isometry3d::Identity(), Eigen::Vector3d::Ones(), 0);

  this->ui->qvtkWidget->SetRenderWindow(m_renderWindow);

  m_renderer->SetBackground(colors->GetColor3d("#212e40").GetData());

  this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

  m_axesWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_axesWidget->SetOrientationMarker(m_axes);
  m_axesWidget->SetInteractor(this->ui->qvtkWidget->GetInteractor());
  m_axesWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
  m_axesWidget->SetEnabled(1);
  m_axesWidget->SetInteractive(0);

//  auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
//  reader->SetFileName("/home/matthieu/dev/CalibrationDepthPose/data/pc_0000.ply");
//  reader->Update();




//  for (int i = 0; i < NB; ++i)
//  {
//    reader->SetFileName(std::string("../data/frame_" + std::to_string(i) + ".vtp").c_str());
//    reader->Update();
//    m_lidarFramesManager.addFrame({i, reader->GetOutput()});
//  }



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


  vtkCamera *cam =  m_renderer->GetActiveCamera();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::update()
{
  auto [first_frame, last_frame] = m_timeStepsManager.getCurrentTimeInterval();
  qDebug() << "interval = " << first_frame << " -> " << last_frame;

  // Display Lidar frames
  // first clean all actors
  for (unsigned int i = 0; i < m_lidarFramesManager.getNbFrames(); ++i)
  {
    m_renderer->RemoveActor(m_pcActors[i]);
  }

  // select skip mode
  int skip_mode = 0;
  if (last_frame - first_frame + 1 >= m_fastRenderingThreshold)
  {
    skip_mode = m_skipFramesMode;
  }

  if (skip_mode == 0 || skip_mode == 4)
  {
    for (int i = first_frame; i <= last_frame; ++i)
    {
      m_renderer->AddActor(m_pcActors[i]);
    }
  }
  else if (skip_mode == 1 || skip_mode == 2)
  {
    int incr = (3 - skip_mode) * 2;
    for (int i = first_frame; i <= last_frame; i += incr)
    {
      m_renderer->AddActor(m_pcActors[i]);
    }
  }
  else if (skip_mode == 3)
  {
    for (int i = first_frame; i <= last_frame; ++i)
    {
      m_renderer->AddActor(m_pcActors[i]);
    }

    for (int i = first_frame; i <= last_frame; i += 4)
    {
      m_renderer->RemoveActor(m_pcActors[i]);
    }
  }
  // Force to display the first and last frames
  m_renderer->AddActor(m_pcActors[last_frame]);
  m_renderer->AddActor(m_pcActors[first_frame]);

  // Display bounding boxes
  for (int i = 0; i < m_bbActors.size(); ++i)
  {
    auto visible = m_boundingBoxManager.getBoundingBoxFromIndex(i)->isPresentInInterval(first_frame, last_frame);
    auto bb_state = m_boundingBoxManager.getBoundingBoxFromIndex(i)->getState();
    auto displayMode = ui->groupBox_DisplayManager->getDisplayMode();
    if (visible && displayMode == DisplayManager_ui::DisplayMode::BOTH
        || bb_state == BoundingBox::State::STATIC && displayMode == DisplayManager_ui::DisplayMode::ONLY_STATIC
        || bb_state == BoundingBox::State::DYNAMIC && displayMode == DisplayManager_ui::DisplayMode::ONLY_DYNAMIC)
    {

      BoundingBox* bbToDisplay = m_boundingBoxManager.getBoundingBoxFromIndex(i);
      const Eigen::Isometry3d& pose = bbToDisplay->getPose(first_frame);
      const Eigen::Vector3d dimensions = bbToDisplay->getDimensions();

      vtkSmartPointer<vtkMatrix4x4> matrix = eigenIsometry3dToVtkMatrix4x4(pose);
      for (int i = 0; i < 3; ++i)
      {
        matrix->SetElement(i, 0, matrix->GetElement(i, 0) * dimensions[0]);
        matrix->SetElement(i, 1, matrix->GetElement(i, 1) * dimensions[1]);
        matrix->SetElement(i, 2, matrix->GetElement(i, 2) * dimensions[2]);
      }
      m_bbActors[i]->SetUserMatrix(matrix);
      m_renderer->AddActor(m_bbActors[i]);
    }
    else
    {
      m_renderer->RemoveActor(m_bbActors[i]);
    }
  }

  // Dislpay the bounding box manipulator widget
  if (m_currentlyEditedBox >= 0)
  {
    if (!m_boundingBoxManager.getBoundingBoxFromIndex(m_currentlyEditedBox)->isPresentInInterval(first_frame, last_frame))
    {
      disableBoxWidget();
    }
  }
  disableBoxWidget();
  m_renderWindow->Render();
}

void MainWindow::initialize()
{
  m_classesManager.loadFromYaml("../classes.yaml");
  ui->widget_BB_Information->updateAvailableClasses(m_classesManager.getAvailableClasses());
  m_timeStepsManager.setModeSingle(0);

//  addBoundingBox(Eigen::Translation3d(0, 0, 0)  * Eigen::Quaterniond::Identity(), Eigen::Vector3d::Ones());
//  addBoundingBox(Eigen::Translation3d(2, -1, 0) * Eigen::Quaterniond::Identity(), Eigen::Vector3d::Ones());
//  addBoundingBox(Eigen::Translation3d(-2, 2, 0) * Eigen::Quaterniond::Identity(), Eigen::Vector3d::Ones());
  openLidarDataset();
}

void MainWindow::displayLog(const QString& msg)
{
  ui->textBrowser_logger->append(msg);
}



void MainWindow::addBoundingBox(const Eigen::Isometry3d& pose, const Eigen::Vector3d& dimensions)
{
  qInfo() << "Add Bounding Box";
  const std::string& initialClass = "car"; // getDefaultClass from ui
  auto currentFrameInterval = m_timeStepsManager.getCurrentTimeInterval();
  auto instanceId = m_boundingBoxManager.findFirstUnusedInstanceId();

  const BoundingBox* bb = m_boundingBoxManager.addBoundingBox(instanceId, initialClass, pose, dimensions, currentFrameInterval);

//  bb->addPresenceInFrame( temp_transl * Eigen::Isometry3d::Identity() * Eigen::EulerAnglesXYZd(10, 20, 30), 0);

  auto source = vtkSmartPointer<vtkCubeSource>::New();
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  auto actor = vtkSmartPointer<vtkActor>::New();
  m_bbSources.emplace_back(source);
  m_bbMappers.emplace_back(mapper);
  m_bbActors.emplace_back(actor);

  mapper->SetInputConnection(source->GetOutputPort());
  actor->SetMapper(mapper);
  actor->SetUserMatrix(eigenIsometry3dToVtkMatrix4x4(pose));
  auto col = m_classesManager.getClassColor(bb->getClass());
  actor->GetProperty()->SetColor(col.r, col.g, col.b);
  actor->GetProperty()->SetRepresentationToSurface();
  actor->GetProperty()->SetOpacity(0.2);
  m_renderer->AddActor(actor);

  // maybe return the corresponding actor so that we can continue to and call edit
}

void MainWindow::createNewBoundingBox()
{
  double focalPoint[3];
  m_renderer->GetActiveCamera()->GetFocalPoint(focalPoint);
  Eigen::Isometry3d pose = Eigen::Isometry3d::Identity();
  pose = Eigen::Translation3d(focalPoint[0], focalPoint[1], focalPoint[2]) * pose;
  Eigen::Vector3d dimension(1.0, 1.0, 1.0);
  addBoundingBox(pose, dimension);
}

int MainWindow::findBoundingBoxFromActor(vtkActor* actor)
{
  int idx = -1;
  for (unsigned int i = 0; i < m_bbActors.size(); ++i)
  {
    if (actor == m_bbActors[i].Get())
    {
      idx = i;
      break;
    }
  }
  return idx;
}

void MainWindow::disableBoxWidget()
{
  ui->widget_BB_Information->unselectBoundingBox();
  m_boxWidget->Off();
  m_currentlyEditedBox = -1;
}

void MainWindow::editBoundingBox(int index)
{
  if (index >= 0)
  {
    auto *bb = m_boundingBoxManager.getBoundingBoxFromIndex(index);

    // update pose
    vtkMatrix4x4 *poseMatrix = m_bbActors[index]->GetUserMatrix();
    Eigen::Vector3d scaling;
    // Get rigid transform + scaling
    Eigen::Isometry3d poseEigen = eigenIsometry3dFromVtkMatrix4x4(poseMatrix, &scaling);
    bb->setDimensions(scaling);
    if (bb->getState() == BoundingBox::State::STATIC)
    {
      const auto& frames = bb->getFrames();
      for (const auto& f : frames)
      {
        bb->setPose(f, poseEigen);
      }
    }
    else
    {
      auto [tFirst, tLast] = m_timeStepsManager.getCurrentTimeInterval();
      for (int t = tFirst; t <= tLast; ++t)
      {
        bb->setPose(t, poseEigen);
      }
    }

//    bb->setClass();
    ui->widget_BB_Information->updateInformation(bb);

    // update color from class
    auto col = m_classesManager.getClassColor(bb->getClass());
    m_bbActors[index]->GetProperty()->SetColor(col.r, col.g, col.b);

    m_renderer->Render();
  }
}

void MainWindow::deleteBoundingBox()
{
  if (m_currentlyEditedBox >= 0)
  {
    auto currentFramesInterval = m_timeStepsManager.getCurrentTimeInterval();
    // bb manager delete bb
    bool isCompletelyRemoved = m_boundingBoxManager.deleteBoundingBox(m_currentlyEditedBox, currentFramesInterval);

    if (isCompletelyRemoved)
    {
      // remove vtk stuff
      m_renderer->RemoveActor(m_bbActors[m_currentlyEditedBox]);
      m_bbActors.erase (m_bbActors.begin() + m_currentlyEditedBox);
      m_bbMappers.erase(m_bbMappers.begin() + m_currentlyEditedBox);
      m_bbSources.erase(m_bbSources.begin() + m_currentlyEditedBox);
      m_currentlyEditedBox = -1;
    }

    disableBoxWidget();
    update();
  }
}

void MainWindow::updateBoundingBoxInstanceId(int index, unsigned int id)
{
  auto* bb = m_boundingBoxManager.getBoundingBoxFromIndex(index);
  if (bb->getInstanceId() != id)
  {
    auto* bbTarget = m_boundingBoxManager.getBoundingBoxFromInstanceId(id);
    if (bbTarget)
    {
      const auto& poses = bb->getPoses();
      const auto& frames = bb->getFrames();
      for (int i = 0; i < poses.size(); ++i)
      {
        bbTarget->addPresenceInFrame(poses[i], frames[i]);
      }

      auto newSelectedActor = m_bbActors[bbTarget->getStoringId()];
      deleteBoundingBox();
      selectBoundingBox(newSelectedActor);
    }
  }
}

void MainWindow::openLidarDataset()
{
  std::cout << "open dataset" << std::endl;
//  auto fileName = QFileDialog::getOpenFileName(this, tr("Open Lidar Dataset"), "../", tr("Series Files (*.series)"));
  QString fileName("/home/matthieu/dev/BoundingBoxMe/20181226_100838_FCR_SBR_AD1_R04_DC_JSOIMPREZA_noADC.pcap.lidarframes/frame.vtp.series");
  std::cout << "selected file " << fileName.toStdString() << std::endl;

  if (loadLidarDataSet(fileName.toStdString()))
  {
    m_timeStepsManager.initializeSize(m_lidarFramesManager.getNbFrames());
    m_timeStepsManager.setModeSingle(0);

    // Update vtk stuff
    auto pointsToRender = m_lidarFramesManager.getFramesPoints();

    auto lookupTable = vtkSmartPointer<vtkColorTransferFunction>::New();
    lookupTable->SetColorSpaceToDiverging();
    lookupTable->AddRGBPoint(0, 0.23137254902000001, 0.298039215686, 0.75294117647100001);
    lookupTable->AddRGBPoint(127, 0.86499999999999999, 0.86499999999999999, 0.86499999999999999);
    lookupTable->AddRGBPoint(255, 0.70588235294099999, 0.015686274509800001, 0.149019607843);
    lookupTable->Build();


    for (auto* pts : pointsToRender)
    {
      auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      mapper->SetLookupTable(lookupTable);
      mapper->SetScalarModeToUsePointFieldData();
      mapper->SetColorModeToMapScalars();
      mapper->SetUseLookupTableScalarRange(1);
      auto vertexFilter = vtkSmartPointer<vtkVertexGlyphFilter>::New();
      auto actor = vtkSmartPointer<vtkActor>::New();
      m_pcMappers.emplace_back(mapper);
      m_pcVertexGlyphFilters.emplace_back(vertexFilter);
      m_pcActors.emplace_back(actor);
      vertexFilter->SetInputData(pts);
      mapper->SetInputConnection(vertexFilter->GetOutputPort());
      mapper->SelectColorArray("intensity");    // TODO: check if exists
      mapper->ScalarVisibilityOn();
      actor->SetMapper(mapper);
      actor->GetProperty()->SetPointSize(2);
    }

    ui->groupBox_TimeSteps_Manager->updateTimeStepsBounds(0, m_lidarFramesManager.getNbFrames());
  }
}

void MainWindow::loadBoundingBoxDataset()
{
//  auto fileName = QFileDialog::getOpenFileName(this, tr("Open Lidar Dataset"), "../", tr("Series Files (*.series)"));
  QString fileName("/home/matthieu/dev/BoundingBoxMe/build/test/bb.series");

  if (loadBBoxDataSet(fileName.toStdString()))
  {
    // create the corresponding actors (one per BBox)
    const auto& BBoxes = m_boundingBoxManager.getBoundingBoxes();

    for (auto bbPtr : BBoxes)
    {
      auto source = vtkSmartPointer<vtkCubeSource>::New();
      auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      auto actor = vtkSmartPointer<vtkActor>::New();
      m_bbSources.emplace_back(source);
      m_bbMappers.emplace_back(mapper);
      m_bbActors.emplace_back(actor);

      mapper->SetInputConnection(source->GetOutputPort());
      actor->SetMapper(mapper);
  //    actor->SetUserMatrix(eigenIsometry3dToVtkMatrix4x4(pose));
      auto col = m_classesManager.getClassColor(bbPtr->getClass());
      actor->GetProperty()->SetColor(col.r, col.g, col.b);
      actor->GetProperty()->SetRepresentationToSurface();
      actor->GetProperty()->SetOpacity(0.2);
      m_renderer->AddActor(actor);
    }
  }
  update();
}

void MainWindow::saveBoundingBoxDataset()
{
  std::cout << "Save bb dataset" << std::endl;
//  auto fileName = QFileDialog::getSaveFileName(this, tr("Save BBox Dataset"), "./", tr("Series Files (*.series)"));
  QString fileName("/home/matthieu/dev/BoundingBoxMe/build/test/bb.series");

  std::cout << "selected file " << fileName.toStdString() << std::endl;

  writeBBoxDataSet(fileName.toStdString());
}

void MainWindow::selectBoundingBox(vtkActor *bbActor)
{
  int idx = findBoundingBoxFromActor(bbActor);
  if (idx != -1)
  {
    m_currentlyEditedBox = idx;
    qInfo() << "Select bounding box" << idx;
    m_boxWidget->SetProp3D(bbActor);
    m_boxWidget->PlaceWidget();
    m_boxWidget->On();

    std::vector<unsigned int> availableIds = { m_boundingBoxManager.getBoundingBoxFromIndex(idx)->getInstanceId() };
    const auto& framesOfPresence = m_boundingBoxManager.getBoundingBoxFromIndex(idx)->getFrames();
    for (int index = 0; index < m_bbActors.size(); ++index)
    {
      if (index != idx)
      {
        auto *bb = m_boundingBoxManager.getBoundingBoxFromIndex(index);
        if (bb->getState() == BoundingBox::State::DYNAMIC)
        {
          const auto& frames = bb->getFrames();
          bool hasNoIntersection = true;
          for (const auto& f : frames)
          {
            if (std::find(framesOfPresence.begin(), framesOfPresence.end(), f) != framesOfPresence.end())
            {
              hasNoIntersection = false;
              break;
            }
          }
          if (hasNoIntersection)
          {
            availableIds.push_back(bb->getInstanceId());
          }
        }
      }
    }
    this->ui->widget_BB_Information->updateAvailableInstanceIds(availableIds);

    this->ui->widget_BB_Information->updateInformation(m_boundingBoxManager.getBoundingBoxFromIndex(idx));
  }
}
