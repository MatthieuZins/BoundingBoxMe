#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
#include <vtkCubeSource.h>
#include <vtkCommand.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVertexGlyphFilter.h>

#include "classesManager.h"
#include "lidarFrameManager.h"
#include "timeStepsManager.h"
#include "boundingBoxManager.h"
#include "vtkBoundingBoxManipulatorWidget.h"

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
    m_renderWindow->Render();
  }

  void addBoundingBox(const Eigen::Isometry3d &pose, const Eigen::Vector3d &dimensions);
  // this function create a new bounding box from camera position
  void createNewBoundingBox();

  void editBoundingBox(int index);

  void deleteBoundingBox();

  void updateBoundingBoxInstanceId(int index, unsigned int id);


  void setFastRenderingThreshold(int th);
  void setSkipFramesMode(int mode);

public slots:
  void openLidarDataset();
  void loadBoundingBoxDataset();
  void saveBoundingBoxDataset();

private:
  Ui::MainWindow *ui;
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkAxesActor> m_axes;
  vtkSmartPointer<vtkOrientationMarkerWidget> m_axesWidget;

  std::vector<vtkSmartPointer<vtkPolyDataMapper>> m_pcMappers;
  std::vector<vtkSmartPointer<vtkActor>> m_pcActors;
  std::vector<vtkSmartPointer<vtkVertexGlyphFilter>> m_pcVertexGlyphFilters;

  std::vector<vtkSmartPointer<vtkCubeSource>> m_bbSources;
  std::vector<vtkSmartPointer<vtkPolyDataMapper>> m_bbMappers;
  std::vector<vtkSmartPointer<vtkActor>> m_bbActors;

  vtkSmartPointer<vtkBoundingBoxManipulatorWidget> m_boxWidget;

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
};


inline void MainWindow::setFastRenderingThreshold(int th)
{
  m_fastRenderingThreshold = th;
}

inline void MainWindow::setSkipFramesMode(int mode)
{
  m_skipFramesMode = mode;
}

#endif // MAINWINDOW_H
