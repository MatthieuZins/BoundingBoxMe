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
#include <vtkCommand.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkVertexGlyphFilter.h>

#include "lidarFrameManager.h"
#include "timeStepsManager.h"

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


  void update();
  // funciton load dataset
  // - should update the min max interval

private:
  Ui::MainWindow *ui;
  vtkSmartPointer<vtkGenericOpenGLRenderWindow> m_renderWindow;
  vtkSmartPointer<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkAxesActor> m_axes;
  vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;

  std::vector<vtkSmartPointer<vtkPolyDataMapper>> m_mappers;
  std::vector<vtkSmartPointer<vtkActor>> m_actors;
  std::vector<vtkSmartPointer<vtkVertexGlyphFilter>> m_vertexGlyphFilters;

};

#endif // MAINWINDOW_H
