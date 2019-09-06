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

  void addBoundingNewBox(const Eigen::Translation3d &temp_transl);

  void selectBoundingBox(vtkActor* bbActor);
  int findBoundingBoxFromActor(vtkProp3D *actor);

  void disableBoxWidget();

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

};

#endif // MAINWINDOW_H
