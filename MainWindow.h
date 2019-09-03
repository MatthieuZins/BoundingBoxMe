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

private:
  Ui::MainWindow *ui;
  LidarFrameManager& m_lidarFramesManager;
  TimeStepsManager& m_timeStepsManager;

  std::vector<vtkSmartPointer<vtkPolyDataMapper>> m_mappers;
  std::vector<vtkSmartPointer<vtkActor>> m_actors;
  std::vector<vtkSmartPointer<vtkVertexGlyphFilter>> m_vertexGlyphFilters;

  vtkNew<vtkRenderer> m_renderer;
  vtkSmartPointer<vtkAxesActor> m_axes;
  vtkSmartPointer<vtkOrientationMarkerWidget> m_widget;
};

#endif // MAINWINDOW_H
