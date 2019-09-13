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

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <vtkInteractorStyleImage.h>
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
#include <vtkPointData.h>
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
#include <QComboBox>
#include <QMessageBox>
#include <QFileDialog>
#include <unsupported/Eigen/EulerAngles>
#include <vtkMatrix4x4.h>
#include "keyPressInteractorStyle.h"
#include "vtkEigenUtils.h"
#include "datasetHelper.h"
#include "vtkBoundingBoxSource.h"

#include "sideViewsInteractorStyle.h"


//Callback for automatic change of interaction styles
class vtkVolumeInteractionCallback : public vtkCommand
{
public:

  static vtkVolumeInteractionCallback *New() {
    return new vtkVolumeInteractionCallback;
  }


  void SetInteractor(vtkRenderWindowInteractor *interactor) {
    this->Interactor = interactor;
  }

  vtkRenderWindowInteractor *GetInteractor() {
    return this->Interactor;
  }

  void SetVolumeInteractorStyle(vtkSmartPointer<KeyPressInteractorStyle> m) {
    this-> m_style3d = m;
  }

  void SetImageInteractorStyle(vtkSmartPointer<SideViewsInteractorStyle> style2,
                               vtkSmartPointer<SideViewsInteractorStyle> style3,
                               vtkSmartPointer<SideViewsInteractorStyle> style4)
  {
    m_style2d_2 = style2;
    m_style2d_3 = style3;
    m_style2d_4 = style4;
  }

  void setRenderers(vtkRenderer* renderer3d, vtkRenderer* renderer2d_2,
                    vtkRenderer* renderer2d_3,vtkRenderer* renderer2d_4){
    m_renderer3d = renderer3d;
    m_renderer2d_2 = renderer2d_2;
    m_renderer2d_3 = renderer2d_3;
    m_renderer2d_4 = renderer2d_4;
  }

  virtual void Execute(vtkObject *, unsigned long event, void *)
  {
    vtkRenderWindowInteractor *interactor = this->GetInteractor();

    int lastPos[2];
    interactor->GetLastEventPosition(lastPos);
    int currPos[2];
    interactor->GetEventPosition(currPos);
    double port[4];


    if (event == vtkCommand::MouseMoveEvent)
    {
       vtkRenderer *renderer = interactor->FindPokedRenderer(currPos[0], currPos[1]);
      if (renderer == m_renderer3d)
      {
        interactor->SetInteractorStyle(m_style3d);
      }
      else if (renderer == m_renderer2d_2)
      {
        interactor->SetInteractorStyle(m_style2d_2);
        m_style2d_2->SetDefaultRenderer(renderer);
      }
      else if (renderer == m_renderer2d_3)
      {
        interactor->SetInteractorStyle(m_style2d_3);
        m_style2d_3->SetDefaultRenderer(renderer);
      }
      else if (renderer == m_renderer2d_4)
      {
        interactor->SetInteractorStyle(m_style2d_4);
        m_style2d_4->SetDefaultRenderer(renderer);
      }

      vtkInteractorStyle *style = vtkInteractorStyle::SafeDownCast(interactor->GetInteractorStyle());
      if (style)
      {
        style->OnMouseMove();
      }
    }
  }

private:
  vtkVolumeInteractionCallback() = default;

  // Pointer to the interactor
  vtkRenderWindowInteractor *Interactor = nullptr;
  //pointer to interactor volume style
  vtkSmartPointer<KeyPressInteractorStyle> m_style3d = nullptr;
  //pointer to interactor image style
  vtkSmartPointer<SideViewsInteractorStyle> m_style2d_2 = nullptr;
  vtkSmartPointer<SideViewsInteractorStyle> m_style2d_3 = nullptr;
  vtkSmartPointer<SideViewsInteractorStyle> m_style2d_4 = nullptr;

 //pointer to image renderer
  vtkRenderer *m_renderer3d = nullptr;
  vtkRenderer *m_renderer2d_2 = nullptr;
  vtkRenderer *m_renderer2d_3 = nullptr;
  vtkRenderer *m_renderer2d_4 = nullptr;
};



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
  m_cameraFramesManager(CameraFramesManager::getInstance()),
  m_renderWindow(vtkSmartPointer<vtkGenericOpenGLRenderWindow>::New()),
  m_renderer(vtkSmartPointer<vtkRenderer>::New()),
  m_renderer2(vtkSmartPointer<vtkRenderer>::New()),
  m_renderer3(vtkSmartPointer<vtkRenderer>::New()),
  m_renderer4(vtkSmartPointer<vtkRenderer>::New()),
  m_axesWidget(vtkSmartPointer<vtkOrientationMarkerWidget>::New()),
  m_axesWidget2(vtkSmartPointer<vtkOrientationMarkerWidget>::New()),
  m_axesWidget3(vtkSmartPointer<vtkOrientationMarkerWidget>::New()),
  m_axesWidget4(vtkSmartPointer<vtkOrientationMarkerWidget>::New()),
  m_boxWidget(vtkSmartPointer<vtkBoundingBoxManipulatorWidget>::New()),
  m_boxWidget2(vtkSmartPointer<vtkBoundingBoxManipulatorWidget>::New()),
  m_boxWidget3(vtkSmartPointer<vtkBoundingBoxManipulatorWidget>::New()),
  m_boxWidget4(vtkSmartPointer<vtkBoundingBoxManipulatorWidget>::New()),
  m_comboBox_ColorBy(std::make_unique<QComboBox>()),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  statusBar()->showMessage("Auto save disabled");


  // Add values in the combo box
  QLabel* label_ColorBy = new QLabel("Color By  ");
  m_comboBox_ColorBy->addItem("Solid Color");
  ui->mainToolBar->addSeparator();
  ui->mainToolBar->addWidget(label_ColorBy);
  ui->mainToolBar->addWidget(m_comboBox_ColorBy.get());

  tabifyDockWidget(ui->dockWidget_Display, ui->dockWidget_Manager);

  // Handle connections
  QObject::connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(openLidarDataset()));
  QObject::connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveBoundingBoxDataset()));
  QObject::connect(ui->actionOpenBBox, SIGNAL(triggered()), this, SLOT(loadBoundingBoxDataset()));
  QObject::connect(ui->actionRestart, SIGNAL(triggered()), this, SLOT(restart()));
  QObject::connect(ui->pushButton_Clear_Logger, SIGNAL(clicked(bool)), this, SLOT(clearLogger()));
  QObject::connect(m_comboBox_ColorBy.get(), SIGNAL(activated(int)), this, SLOT(chooseColorArrayUsed(int)));
  QObject::connect(ui->actionOpenCameraFrames, SIGNAL(triggered()), this, SLOT(loadCameraFramesDataset()));


  /// Setup the time for AutoSave
  m_autoSaveTimer = std::make_unique<QTimer>(this);
  QObject::connect(m_autoSaveTimer.get(), SIGNAL(timeout()), this, SLOT(autoSaveBoundingBoxDataset()));
  m_autoSaveTimer->start(m_autoSaveFreq_msec);


  this->ui->qvtkWidget->SetRenderWindow(m_renderWindow);

  vtkNew<vtkNamedColors> colors;
  vtkColor3ub colBackground = colors->HTMLColorToRGB(m_backgroundColor);
  vtkColor3ub colSideViews = colors->HTMLColorToRGB(m_backgroundColorSideViews);


  // Main Renderer
  m_renderer->SetUseDepthPeeling(true);
  m_renderer->SetBackground(static_cast<double>(colBackground.GetRed()) / 255,
                            static_cast<double>(colBackground.GetGreen()) / 255,
                            static_cast<double>(colBackground.GetBlue()) / 255);

  m_renderWindow->AddRenderer(m_renderer);
  m_renderer->SetViewport(0.0, m_sideViewerHeight, 1.0, 1.0);
  m_axesWidget->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_axesWidget->SetOrientationMarker(vtkSmartPointer<vtkAxesActor>::New());
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



  auto style3d = vtkSmartPointer<KeyPressInteractorStyle>::New();
  style3d->SetDefaultRenderer(m_renderer);
  style3d->setMainWindow(this);
  style3d->setRenderer(m_renderer);
  ui->qvtkWidget->GetInteractor()->SetInteractorStyle(style3d);

  m_renderer->GetActiveCamera()->SetPosition(0.0, 0.0, 40.0);


  // Renderer 2
  auto style2d_2 = vtkSmartPointer<SideViewsInteractorStyle>::New();
  style2d_2->SetDefaultRenderer(m_renderer2);
  style2d_2->setMainWindow(this);
  style2d_2->setRenderer(m_renderer2);

  m_renderWindow->AddRenderer(m_renderer2);
  m_renderer2->SetUseDepthPeeling(true);
  m_renderer2->SetViewport(0, 0, 0.33, m_sideViewerHeight);
  m_renderer2->SetBackground(static_cast<double>(colSideViews.GetRed()) / 255,
                            static_cast<double>(colSideViews.GetGreen()) / 255,
                            static_cast<double>(colSideViews.GetBlue()) / 255);
  m_axesWidget2->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_axesWidget2->SetOrientationMarker(vtkSmartPointer<vtkAxesActor>::New());
  m_axesWidget2->SetInteractor(this->ui->qvtkWidget->GetInteractor());
  m_axesWidget2->SetViewport(0.0, 0.0, 0.3, 0.3);
  m_axesWidget2->SetEnabled(1);
  m_axesWidget2->SetInteractive(0);
  auto* cam2 = m_renderer2->GetActiveCamera();
  cam2->SetPosition(40.0, 0.0, 0.0);
  cam2->SetFocalPoint(0.0, 0.0, 0.0);
  cam2->SetViewUp(0.0, 0.0, 1.0);
  cam2->SetParallelProjection(true);
  cam2->SetParallelScale(10.0);
  auto callback2 = vtkSmartPointer<vtkMyCallback>::New();
  callback2->setMainWindow(this);
  m_boxWidget2->SetInteractor(m_renderWindow->GetInteractor());
  m_boxWidget2->AddObserver(vtkCommand::InteractionEvent, callback2);
  m_boxWidget2->SetPlaceFactor(1.0);
  m_boxWidget2->Off();

  // Renderer 3
  auto style2d_3 = vtkSmartPointer<SideViewsInteractorStyle>::New();
  style2d_3->SetDefaultRenderer(m_renderer3);
  style2d_3->setMainWindow(this);
  style2d_3->setRenderer(m_renderer3);
  m_renderWindow->AddRenderer(m_renderer3);
  m_renderer3->SetUseDepthPeeling(true);
  m_renderer3->SetViewport(0.33, 0, 0.66, m_sideViewerHeight);
  m_renderer3->SetBackground(static_cast<double>(colSideViews.GetRed()) / 255,
                            static_cast<double>(colSideViews.GetGreen()) / 255,
                            static_cast<double>(colSideViews.GetBlue()) / 255);
  m_axesWidget3->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_axesWidget3->SetOrientationMarker(vtkSmartPointer<vtkAxesActor>::New());
  m_axesWidget3->SetInteractor(this->ui->qvtkWidget->GetInteractor());
  m_axesWidget3->SetViewport(0.0, 0.0, 0.3, 0.3);
  m_axesWidget3->SetEnabled(1);
  m_axesWidget3->SetInteractive(0);
  auto* cam3 = m_renderer3->GetActiveCamera();
  cam3->SetPosition(0.0, 0.0, 40.0);
  cam3->SetFocalPoint(0.0, 0.0, 0.0);
  cam3->SetViewUp(0.0, 1.0, 0.0);
  cam3->SetParallelProjection(true);
  cam3->SetParallelScale(20.0);
  auto callback3 = vtkSmartPointer<vtkMyCallback>::New();
  callback3->setMainWindow(this);
  m_boxWidget3->SetInteractor(m_renderWindow->GetInteractor());
  m_boxWidget3->AddObserver(vtkCommand::InteractionEvent, callback3);
  m_boxWidget3->SetPlaceFactor(1.0);
  m_boxWidget3->Off();

  // Renderer 4
  auto style2d_4 = vtkSmartPointer<SideViewsInteractorStyle>::New();
  style2d_4->SetDefaultRenderer(m_renderer4);
  style2d_4->setMainWindow(this);
  style2d_4->setRenderer(m_renderer4);
  m_renderWindow->AddRenderer(m_renderer4);
  m_renderer4->SetUseDepthPeeling(true);
  m_renderer4->SetViewport(0.66, 0, 1.0, m_sideViewerHeight);
  m_renderer4->SetBackground(static_cast<double>(colSideViews.GetRed()) / 255,
                            static_cast<double>(colSideViews.GetGreen()) / 255,
                            static_cast<double>(colSideViews.GetBlue()) / 255);
  m_axesWidget4->SetOutlineColor(0.9300, 0.5700, 0.1300);
  m_axesWidget4->SetOrientationMarker(vtkSmartPointer<vtkAxesActor>::New());
  m_axesWidget4->SetInteractor(this->ui->qvtkWidget->GetInteractor());
  m_axesWidget4->SetViewport(0.0, 0.0, 0.3, 0.3);
  m_axesWidget4->SetEnabled(1);
  m_axesWidget4->SetInteractive(0);
  auto* cam4 = m_renderer4->GetActiveCamera();
  cam4->SetPosition(0.0, 40.0, 0.0);
  cam4->SetFocalPoint(0.0, 0.0, 0.0);
  cam4->SetViewUp(0.0, 0.0, 1.0);
  cam4->SetParallelProjection(true);
  cam4->SetParallelScale(10.0);
  auto callback4 = vtkSmartPointer<vtkMyCallback>::New();
  callback4->setMainWindow(this);
  m_boxWidget4->SetInteractor(m_renderWindow->GetInteractor());
  m_boxWidget4->AddObserver(vtkCommand::InteractionEvent, callback4);
  m_boxWidget4->SetPlaceFactor(1.0);
  m_boxWidget4->Off();




  // call back to change style interaction according to the renderer
  auto callbackSwitchInteraction = vtkSmartPointer<vtkVolumeInteractionCallback>::New();
  callbackSwitchInteraction->SetInteractor(ui->qvtkWidget->GetInteractor());
  callbackSwitchInteraction->SetImageInteractorStyle(style2d_2, style2d_3, style2d_4);
  callbackSwitchInteraction->setRenderers(m_renderer, m_renderer2, m_renderer3, m_renderer4);
  callbackSwitchInteraction->SetVolumeInteractorStyle(style3d);
  ui->qvtkWidget->GetInteractor()->AddObserver(vtkCommand::MouseMoveEvent, callbackSwitchInteraction);

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
    safeRemoveLidarActor(i);
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
      safeAddLidarActor(i);
    }
  }
  else if (skip_mode == 1 || skip_mode == 2)
  {
    int incr = (3 - skip_mode) * 2;
    for (int i = first_frame; i <= last_frame; i += incr)
    {
      safeAddLidarActor(i);
    }
  }
  else if (skip_mode == 3)
  {
    for (int i = first_frame; i <= last_frame; ++i)
    {
      safeAddLidarActor(i);
    }

    for (int i = first_frame; i <= last_frame; i += 4)
    {
      safeAddLidarActor(i);
    }
  }
  // Force to display the first and last frames
  safeAddLidarActor(last_frame);
  safeAddLidarActor(first_frame);

  for (vtkPolyDataMapper *mapper : m_pcMappers)
  {
    mapper->SelectColorArray(m_comboBox_ColorBy->currentText().toStdString().c_str());
    mapper->ScalarVisibilityOn();
  }

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

      safeAddBBoxActor(i);
    }
    else
    {
      safeRemoveBBoxActor(i);
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
  // first try to load it where the executable is
  if (!m_classesManager.loadFromYaml("classes.yaml"))
  {
    // if it failed, try to load it from the parent directory
    if (!m_classesManager.loadFromYaml("../classes.yaml"))
    {
      bool ok = false;
      while (!ok)
      {
        // if it failed, load it manually
        auto fileName = QFileDialog::getOpenFileName(this, tr("Open Classe Config"), "../", tr("YAML (*.yaml *.yml *.YAML *.YML)")).toStdString();
        ok = m_classesManager.loadFromYaml(fileName);
        if (!ok)
        {
          QMessageBox::StandardButton ret = QMessageBox::warning(this, "Error",
                       tr("Impossible to load the classes from this file.\n"),
                       QMessageBox::Retry | QMessageBox::Close,
                       QMessageBox::Retry);
          if (ret == QMessageBox::Close)
          {
            exit(0);
          }
        }
      }
    }
  }
  qInfo() << "Successfully loaded classes:";
  for (const auto& cl : m_classesManager.getAvailableClasses())
    qInfo() << QString::fromStdString(cl);
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
  m_renderer->AddActor(actor); //TODO: probably remove

  update();
  selectBoundingBox(actor);

  // if we add at least one BBox => disable the button load BBox to avoid strange behaviour
  ui->actionOpenBBox->setDisabled(true);
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
  m_boxWidget2->Off();
  m_boxWidget3->Off();
  m_boxWidget4->Off();
  m_currentlyEditedBox = -1;
}

void MainWindow::editBoundingBox(int index)
{
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

  if (fileName.size() > 0)
  {
    if (loadLidarDataSet(fileName.toStdString()))
    {
      qInfo() << "Successfully loaded Lidar dataset:\t\t" << fileName;

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

      std::unordered_map<std::string, int> mapAvailableArrays;

      for (auto* pts : pointsToRender)
      {
        for (int arrayId = 0; arrayId < pts->GetPointData()->GetNumberOfArrays(); ++arrayId)
        {
          vtkDataArray* array = pts->GetPointData()->GetArray(arrayId);
          std::string arrayName(array->GetName());
          if (mapAvailableArrays.find(arrayName) == mapAvailableArrays.end())
          {
            mapAvailableArrays[arrayName] = 1;
          }
          else
          {
            mapAvailableArrays[arrayName]++;
          }
        }
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
//        mapper->SelectColorArray("intensity");    // TODO: check if exists
        mapper->ScalarVisibilityOn();
        actor->SetMapper(mapper);
        actor->GetProperty()->SetPointSize(1);
      }

      // keep only the arrays that are available in every point cloud
      std::vector<std::string> goodArrays;
      for (auto it = mapAvailableArrays.begin();  it != mapAvailableArrays.end(); ++it)
      {
        if (it->second == pointsToRender.size())
        {
          goodArrays.push_back(it->first);
        }
      }
      updateColorByArrays(goodArrays);

      ui->groupBox_TimeSteps_Manager->updateTimeStepsBounds(0, m_lidarFramesManager.getNbFrames());
      update();
      ui->actionOpen->setDisabled(true);
    }
  }
}

void MainWindow::loadBoundingBoxDataset()
{
  if (ui->actionOpen->isEnabled())
  {
    qWarning() << "You need to load the the lidar dataset first\n";
    return;
  }
  auto fileName = QFileDialog::getOpenFileName(this, tr("Open Lidar Dataset"), "../", tr("Series Files (*.series)"));
//  QString fileName("/home/matthieu/dev/BoundingBoxMe/build/test/bb.series");
  if (fileName.size() > 0)
  {
    if (loadBBoxDataSet(fileName.toStdString()))
    {
      qInfo() << "Successfully loaded " << m_boundingBoxManager.getBoundingBoxes().size()
              <<  "BBoxes from dataset:\t\t" << fileName;
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
        m_renderer->AddActor(actor);  //TODO: probably remove
      }
      ui->actionOpenBBox->setDisabled(true);
      update();
    }
  }
}

void MainWindow::loadCameraFramesDataset()
{
  if (ui->actionOpen->isEnabled())
  {
    qWarning() << "You need to load the the lidar dataset first\n";
    return;
  }
  auto fileName = QFileDialog::getOpenFileName(this, tr("Open Camera Dataset"), "../", tr("Series Files (*.series)"));
//  QString fileName("/home/matthieu/dev/BoundingBoxMe/build/test/bb.series");
  if (fileName.size() > 0)
  {
    if (loadCameraDataSet(fileName.toStdString()))
    {
      qInfo() << "Successfully loaded camera frames :\t\t" << fileName;
      ui->actionOpenCameraFrames->setDisabled(true);
    }
  }
  // display the frame
  updateCameraFrameDisplayed();
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
    writeBBoxDataSet(m_autoSaveOutputFile);
    statusBar()->showMessage(QString::fromStdString("Auto save: " + m_autoSaveOutputFile));
    qInfo() << "Successfully saved BBox dataset:\t\t" << QString::fromStdString(m_autoSaveOutputFile);
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

void MainWindow::clearLogger()
{
  ui->textBrowser_logger->clear();
}

void MainWindow::chooseColorArrayUsed(int index)
{
  update();
}

void MainWindow::updateColorByArrays(const std::vector<std::string> &arrays)
{
  int defaultIndex = 0;
  size_t idx = 1;
  for (const auto& name : arrays)
  {
    if (name == "intensity" || name == "Intensity")
      defaultIndex = idx;
    m_comboBox_ColorBy->addItem(QString::fromStdString(name));
    ++idx;
  }
  m_comboBox_ColorBy->setCurrentIndex(defaultIndex);
}

void MainWindow::updateCameraFrameDisplayed()
{
  auto [first, last] = m_timeStepsManager.getCurrentTimeInterval();
  const auto& file = m_cameraFramesManager.getFrame(m_lidarFramesManager.getFrameTimestamp(first));
  if (file.size() > 0)
  {
    QPixmap pm(file.c_str());
    ui->label_Frame->setScaledContents(true);
    ui->label_Frame->setPixmap(pm.scaledToWidth(300, Qt::SmoothTransformation));
  }
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

    m_boxWidget2->SetProp3D(bbActor);
    m_boxWidget2->PlaceWidget();
    m_boxWidget2->On();

    m_boxWidget3->SetProp3D(bbActor);
    m_boxWidget3->PlaceWidget();
    m_boxWidget3->On();

    m_boxWidget4->SetProp3D(bbActor);
    m_boxWidget4->PlaceWidget();
    m_boxWidget4->On();


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
