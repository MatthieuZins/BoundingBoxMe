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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <vtkCaptionActor2D.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkCamera.h>
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
#include <vtkCubeSource.h>
#include <vtkCommand.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVertexGlyphFilter.h>
#include <QTimer>
#include <QCloseEvent>
#include <QComboBox>
#include "classesManager.h"
#include "lidarFrameManager.h"
#include "timeStepsManager.h"
#include "boundingBoxManager.h"
#include "vtkBoundingBoxManipulatorWidget.h"
#include "cameraFramesManager.h"


#define RESTART_CODE 3

namespace Ui {
  class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT


public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

  LidarFrameManager& m_lidarFramesManager;
  TimeStepsManager& m_timeStepsManager;
  ClassesManager& m_classesManager;
  BoundingBoxManager& m_boundingBoxManager;
  CameraFramesManager& m_cameraFramesManager;


  void update();
  // funciton load dataset
  // - should update the min max interval

  /// This function initialize the application
  /// For example, it loads the classes available
  void initialize();

  void displayLog(const QString &msg);


  void selectBoundingBox(vtkActor* bbActor);
  int findBoundingBoxFromActor(vtkActor *actor);

  void disableBoxWidget();

  void forceRender() {
    update();
  }

  void addBoundingBox(const Eigen::Isometry3d &pose, const Eigen::Vector3d &dimensions, BoundingBox::State state);
  // this function create a new bounding box from camera position
  void createNewBoundingBox(BoundingBox::State state);

  // edit only the BBox pose and dimensions
  void editBoundingBox(int index);

  void deleteBoundingBox(bool allFrames);

  void updateBoundingBoxInstanceId(int index, unsigned int id);


  void setFastRenderingThreshold(int th);
  void setSkipFramesMode(int mode);

  void moveMainCameraTo(double newFocalPoint[3]);
  void moveSideCamerasTo(double newFocalPoint[3]);

  void updateColorByArrays(const std::vector<std::string>& arrays);

  void updateCameraFrameDisplayed();

public slots:
  void openLidarDataset();
  void loadBoundingBoxDataset();
  void loadCameraFramesDataset();
  void saveBoundingBoxDataset();
  void autoSaveBoundingBoxDataset();

  // Override closeEvent to offer a last chance of saving the work
  void closeEvent(QCloseEvent *event) override;

  // Restart application
  void restart();

  // Clear logger
  void clearLogger();

  // Choose which array to use for coloring
  void chooseColorArrayUsed(int index);


private:
  Ui::MainWindow *ui;
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
  vtkSmartPointer<vtkRenderer> m_renderer, m_renderer2, m_renderer3, m_renderer4;
  vtkSmartPointer<vtkOrientationMarkerWidget> m_axesWidget, m_axesWidget2, m_axesWidget3, m_axesWidget4;

  std::vector<vtkSmartPointer<vtkPolyDataMapper>> m_pcMappers;
  std::vector<vtkSmartPointer<vtkActor>> m_pcActors;
  std::vector<vtkSmartPointer<vtkVertexGlyphFilter>> m_pcVertexGlyphFilters;

  std::vector<vtkSmartPointer<vtkCubeSource>> m_bbSources;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> m_bbMappers;
  std::vector<vtkSmartPointer<vtkActor>> m_bbActors;
  std::vector<vtkSmartPointer<vtkCaptionActor2D>> m_bbCaptions;

  vtkSmartPointer<vtkBoundingBoxManipulatorWidget> m_boxWidget;
  vtkSmartPointer<vtkBoundingBoxManipulatorWidget> m_boxWidget2;
  vtkSmartPointer<vtkBoundingBoxManipulatorWidget> m_boxWidget3;
  vtkSmartPointer<vtkBoundingBoxManipulatorWidget> m_boxWidget4;

  std::unique_ptr<QComboBox> m_comboBox_ColorBy;

  int m_currentlyEditedBox = -1;

  // Fast rendering parameters
  int m_fastRenderingThreshold = 1500;    // fast rendering is enabled only if more frames
                                          // than this value are tried to be displayed
  // 0 -> display all
  // 1 -> display 25% (1 out of 4)
  // 2 -> display 50% (1 out of 2)
  // 3 -> display 75% (3 out of 4)
  // 4 -> display all
  int m_skipFramesMode = 0;


  // Cosmetic Parameters
  double m_boundingBoxOpacity = 0.5;
  const std::string m_backgroundColor = "#202333";
  const std::string m_backgroundColorSideViews = "#182030";
  int m_PointSize = 1;

  double m_sideViewerHeight = 0.4;

  // Auto saving Parameters
  std::string m_autoSaveOutputFile = "";
  int m_autoSaveFreq_msec = 120000;       // 3 minutes
  bool m_autoSaveOn = false;
  std::unique_ptr<QTimer> m_autoSaveTimer = nullptr;


  /// Helper functions to safely add/remove an actor of a pointcloud/BBox actor in all the renderers
  void safeAddLidarActor(int index);
  void safeAddBBoxActor(int index);
  void safeRemoveLidarActor(int index);
  void safeRemoveBBoxActor(int index);
};


inline void MainWindow::setFastRenderingThreshold(int th)
{
  m_fastRenderingThreshold = th;
}

inline void MainWindow::setSkipFramesMode(int mode)
{
  m_skipFramesMode = mode;
}


inline void MainWindow::safeAddLidarActor(int index)
{
  if (index >= 0 && index < m_pcActors.size())
  {
    m_renderer->AddActor(m_pcActors[index]);
    m_renderer2->AddActor(m_pcActors[index]);
    m_renderer3->AddActor(m_pcActors[index]);
    m_renderer4->AddActor(m_pcActors[index]);
  }
}

inline void MainWindow::safeAddBBoxActor(int index)
{
  if (index >= 0 && index < m_bbActors.size())
  {
    m_renderer->AddActor(m_bbActors[index]);
    m_renderer2->AddActor(m_bbActors[index]);
    m_renderer3->AddActor(m_bbActors[index]);
    m_renderer4->AddActor(m_bbActors[index]);
    m_renderer->AddActor(m_bbCaptions[index]);
    m_renderer2->AddActor(m_bbCaptions[index]);
    m_renderer3->AddActor(m_bbCaptions[index]);
    m_renderer4->AddActor(m_bbCaptions[index]);
  }
}

inline void MainWindow::safeRemoveLidarActor(int index)
{
  if (index >= 0 && index < m_pcActors.size())
  {
    m_renderer->RemoveActor(m_pcActors[index]);
    m_renderer2->RemoveActor(m_pcActors[index]);
    m_renderer3->RemoveActor(m_pcActors[index]);
    m_renderer4->RemoveActor(m_pcActors[index]);
  }
}

inline void MainWindow::safeRemoveBBoxActor(int index)
{
  if (index >= 0 && index < m_bbActors.size())
  {
    m_renderer->RemoveActor(m_bbActors[index]);
    m_renderer2->RemoveActor(m_bbActors[index]);
    m_renderer3->RemoveActor(m_bbActors[index]);
    m_renderer4->RemoveActor(m_bbActors[index]);
    m_renderer->RemoveActor(m_bbCaptions[index]);
    m_renderer2->RemoveActor(m_bbCaptions[index]);
    m_renderer3->RemoveActor(m_bbCaptions[index]);
    m_renderer4->RemoveActor(m_bbCaptions[index]);
  }
}

#endif // MAINWINDOW_H
