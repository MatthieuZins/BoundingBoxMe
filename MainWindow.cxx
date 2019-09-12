#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vtkRenderWindowInteractor.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkNamedColors.h>
#include <vtkCamera.h>
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
#include <QMessageBox>
#include <QFileDialog>
#include <unsupported/Eigen/EulerAngles>
#include <vtkMatrix4x4.h>
#include "keyPressInteractorStyle.h"
#include "vtkEigenUtils.h"
#include "datasetHelper.h"
#include "vtkBoundingBoxSource.h"

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
  statusBar()->showMessage("Auto save disabled");

  tabifyDockWidget(ui->dockWidget_Display, ui->dockWidget_Manager);

  QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openLidarDataset()));
  QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveBoundingBoxDataset()));
  QObject::connect(ui->actionOpenBBox, SIGNAL(triggered()), this, SLOT(loadBoundingBoxDataset()));
  QObject::connect(ui->actionRestart, SIGNAL(triggered()), this, SLOT(restart()));


  /// Setup the time for AutoSave
  m_autoSaveTimer = std::make_unique<QTimer>(this);
  QObject::connect(m_autoSaveTimer.get(), SIGNAL(timeout()), this, SLOT(autoSaveBoundingBoxDataset()));
  m_autoSaveTimer->start(m_autoSaveFreq_msec);


  vtkNew<vtkNamedColors> colors;
  this->ui->qvtkWidget->SetRenderWindow(m_renderWindow);

  vtkColor3ub col = colors->HTMLColorToRGB(m_backgroundColor);
  m_renderer->SetUseDepthPeeling(true);
  m_renderer->SetBackground(static_cast<double>(col.GetRed()) / 255,
                            static_cast<double>(col.GetGreen()) / 255,
                            static_cast<double>(col.GetBlue()) / 255);

  this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

  m_axesWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_axesWidget->SetOrientationMarker(m_axes);
  m_axesWidget->SetInteractor(this->ui->qvtkWidget->GetInteractor());
  m_axesWidget->SetViewport(0.0, 0.0, 0.2, 0.2);
  m_axesWidget->SetEnabled(1);
  m_axesWidget->SetInteractive(0);


  auto callback = vtkSmartPointer<vtkMyCallback>::New();
  callback->setMainWindow(this);
  m_boxWidget->SetInteractor(m_renderWindow->GetInteractor());
  m_boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);
  m_boxWidget->SetPlaceFactor(1.0);
  m_boxWidget->Off();

  auto style3 = vtkSmartPointer<KeyPressInteractorStyle>::New();
  style3->SetDefaultRenderer(m_renderer);
  style3->setMainWindow(this);
  ui->qvtkWidget->GetInteractor()->SetInteractorStyle(style3);

  vtkCamera *camera = m_renderer->GetActiveCamera();
  camera->SetPosition(0.0, 0.0, 40.0);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::update()
{
  // in this function we can add/remove the same actor multiple times
  // it is ok because VTK checks if this actor was already added/removed

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
    BoundingBox* bbToDisplay = m_boundingBoxManager.getBoundingBoxFromIndex(i);
    int firstFrameOfPresence = bbToDisplay->getFirstFrameOfPresenceInInterval(first_frame, last_frame);
    auto bb_state = bbToDisplay->getState();
    auto displayMode = ui->widget_DisplayManager->getDisplayMode();
    // update color
    auto color = m_classesManager.getClassColord(bbToDisplay->getClass());
    m_bbActors[i]->GetProperty()->SetColor(color.r, color.g, color.b);

    bool isStaticAndOnlyStatic = bb_state == BoundingBox::State::STATIC && displayMode == DisplayManager_ui::DisplayMode::ONLY_STATIC;
    bool isDynAndOnlyDyn = bb_state == BoundingBox::State::DYNAMIC && displayMode == DisplayManager_ui::DisplayMode::ONLY_DYNAMIC;
    if (firstFrameOfPresence >= 0 && (displayMode == DisplayManager_ui::DisplayMode::BOTH || isStaticAndOnlyStatic || isDynAndOnlyDyn))
    {
      const Eigen::Isometry3d& pose = bbToDisplay->getPose(firstFrameOfPresence);
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

  // Hide the BBox manipulator widget
  if (m_currentlyEditedBox >= 0)
  {
    if (!m_boundingBoxManager.getBoundingBoxFromIndex(m_currentlyEditedBox)->isPresentInInterval(first_frame, last_frame))
    {
      disableBoxWidget();
    }
  }

  m_boxWidget->forceRepresentationToSurface();
  m_renderWindow->Render();
}

void MainWindow::initialize()
{
  m_classesManager.loadFromYaml("../classes.yaml");
  ui->groupBox_BB_Information->updateAvailableClasses(m_classesManager.getAvailableClasses());
  m_timeStepsManager.setModeSingle(0);
}

void MainWindow::displayLog(const QString& msg)
{
  ui->textBrowser_logger->append(msg);
}



void MainWindow::addBoundingBox(const Eigen::Isometry3d& pose, const Eigen::Vector3d& dimensions, BoundingBox::State state)
{
  qInfo() << "Add Bounding Box";
  const std::string& initialClass = ui->groupBox_BB_Information->getCurrentlySelectedClass(); // getDefaultClass from ui
  auto currentFrameInterval = m_timeStepsManager.getCurrentTimeInterval();
  auto instanceId = m_boundingBoxManager.findFirstUnusedInstanceId();

  m_boundingBoxManager.addBoundingBox(instanceId, initialClass, pose, dimensions, currentFrameInterval, state);

  auto source = vtkSmartPointer<vtkBoundingBoxSource>::New();
  auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  auto actor = vtkSmartPointer<vtkActor>::New();
  m_bbSources.emplace_back(source);
  m_bbMappers.emplace_back(mapper);
  m_bbActors.emplace_back(actor);

  mapper->SetInputConnection(source->GetOutputPort());
  actor->SetMapper(mapper);
  actor->SetUserMatrix(eigenIsometry3dToVtkMatrix4x4(pose));
  actor->GetProperty()->SetOpacity(m_boundingBoxOpacity);
  actor->GetProperty()->SetAmbient(1.0);
  m_renderer->AddActor(actor);

  update();
  selectBoundingBox(actor);
}

void MainWindow::createNewBoundingBox(BoundingBox::State state)
{
  double focalPoint[3];
  m_renderer->GetActiveCamera()->GetFocalPoint(focalPoint);
  Eigen::Isometry3d pose = Eigen::Isometry3d::Identity();
  pose = Eigen::Translation3d(focalPoint[0], focalPoint[1], focalPoint[2]) * pose;
  Eigen::Vector3d dimension(1.0, 1.0, 1.0);
  addBoundingBox(pose, dimension, state);
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
  ui->groupBox_BB_Information->unselectBoundingBox();
  m_boxWidget->Off();
  m_currentlyEditedBox = -1;
}

void MainWindow::editBoundingBox(int index)
{
  std::cout << "EDIT BBOox" << std::endl;
  if (index >= 0)
  {
    auto *bb = m_boundingBoxManager.getBoundingBoxFromIndex(index);

    vtkMatrix4x4 *poseMatrix = m_bbActors[index]->GetUserMatrix();
    Eigen::Vector3d scaling;
    // Extract rigid transform and scaling
    Eigen::Isometry3d poseEigen = eigenIsometry3dFromVtkMatrix4x4(poseMatrix, &scaling);
    // Update scaling
    bb->setDimensions(scaling);
    if (bb->getState() == BoundingBox::State::STATIC)
    {
      // update the pose in all the frames where it is present
      const auto& frames = bb->getFrames();
      for (const auto& f : frames)
      {
        bb->setPose(f, poseEigen);
      }
    }
    else
    {
      // update the pose only in the current time interval
      auto [tFirst, tLast] = m_timeStepsManager.getCurrentTimeInterval();
      for (int t = tFirst; t <= tLast; ++t)
      {
        bb->setPose(t, poseEigen);
      }
    }

//    bb->setClass();
    ui->groupBox_BB_Information->updateInformation(bb);

    // update color from class
//    auto col = m_classesManager.getClassColor(bb->getClass());
//    m_bbActors[index]->GetProperty()->SetColor(col.r, col.g, col.b);

//    m_renderer->Render();
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
  auto fileName = QFileDialog::getOpenFileName(this, tr("Open Lidar Dataset"), "../", tr("Series Files (*.series)"));
//  QString fileName("/home/matthieu/dev/BoundingBoxMe/small_dataset/frame.vtp.series");

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
    update();
  }
}

void MainWindow::loadBoundingBoxDataset()
{
  auto fileName = QFileDialog::getOpenFileName(this, tr("Open Lidar Dataset"), "../", tr("Series Files (*.series)"));
//  QString fileName("/home/matthieu/dev/BoundingBoxMe/build/test/bb.series");

  if (loadBBoxDataSet(fileName.toStdString()))
  {
    // create the corresponding actors (one per BBox)
    const auto& BBoxes = m_boundingBoxManager.getBoundingBoxes();

    for (auto bbPtr : BBoxes)
    {
      auto source = vtkSmartPointer<vtkBoundingBoxSource>::New();
      auto mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      auto actor = vtkSmartPointer<vtkActor>::New();
      m_bbSources.emplace_back(source);
      m_bbMappers.emplace_back(mapper);
      m_bbActors.emplace_back(actor);

      mapper->SetInputConnection(source->GetOutputPort());
      actor->SetMapper(mapper);
      actor->GetProperty()->SetOpacity(m_boundingBoxOpacity);
      actor->GetProperty()->SetAmbient(1.0);
      m_renderer->AddActor(actor);
    }
  }
  update();
}

void MainWindow::saveBoundingBoxDataset()
{
  m_autoSaveOutputFile = QFileDialog::getSaveFileName(this, tr("Save BBox Dataset"), "./", tr("Series Files (*.series)")).toStdString();
  if (m_autoSaveOutputFile.size() > 0)
  {
    // force the extension
    fs::path filename(m_autoSaveOutputFile);
    filename.replace_extension("series");
    m_autoSaveOutputFile = filename.generic_string();
    m_autoSaveOn = true;
    statusBar()->showMessage(QString::fromStdString("Auto save: " + m_autoSaveOutputFile));
    writeBBoxDataSet(m_autoSaveOutputFile);
  }
}

void MainWindow::autoSaveBoundingBoxDataset()
{
  if (m_autoSaveOn)
  {
    writeBBoxDataSet(m_autoSaveOutputFile);
  }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (m_boundingBoxManager.getBoundingBoxes().size() > 0)
  {
    QMessageBox::StandardButton ret = QMessageBox::warning(this, "Warning",
                 tr("Some of your work might no be saved. Are you sure to quit ?\n"),
                 QMessageBox::Yes | QMessageBox::Save| QMessageBox::Cancel,
                 QMessageBox::Yes);
    if (ret == QMessageBox::Save)
    {
        saveBoundingBoxDataset();
        event->accept();
    }
    else if (ret == QMessageBox::Cancel)
    {
      event->ignore();
    }
    else
    {
      event->accept();
    }
  }
}

void MainWindow::restart()
{
  if (m_boundingBoxManager.getBoundingBoxes().size() > 0)
  {
    QMessageBox::StandardButton ret = QMessageBox::warning(this, "Warning",
                 tr("Some of your work might no be saved. Are you sure to restart ?\n"),
                 QMessageBox::Yes | QMessageBox::Save| QMessageBox::Cancel,
                 QMessageBox::Yes);
    if (ret == QMessageBox::Save)
    {
        saveBoundingBoxDataset();
    }
    else if (ret == QMessageBox::Cancel)
    {
      return;
    }
  }
  // Need to force the singletons to release
  BoundingBoxManager::releaseInstance();
  ClassesManager::releaseInstance();
  LidarFrameManager::releaseInstance();
  TimeStepsManager::releaseInstance();
  QApplication::exit(RESTART_CODE);
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
    this->ui->groupBox_BB_Information->updateAvailableInstanceIds(availableIds);

    this->ui->groupBox_BB_Information->updateInformation(m_boundingBoxManager.getBoundingBoxFromIndex(idx));
  }
}
