#include "vtkBoundingBoxManipulatorWidget.h"

#include <vtkActor.h>
#include <vtkMatrix4x4.h>
#include <vtkObjectFactory.h>
#include <vtkPoints.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkMath.h>
#include <vtkTransform.h>
#include <vtkSmartPointer.h>

#include<Eigen/Dense>
#include <unsupported/Eigen/EulerAngles>

vtkStandardNewMacro(vtkBoundingBoxManipulatorWidget);

vtkBoundingBoxManipulatorWidget::vtkBoundingBoxManipulatorWidget()
  : userMatrix(vtkSmartPointer<vtkMatrix4x4>::New())
{

}

//vtkBoundingBoxManipulatorWidget::~vtkBoundingBoxManipulatorWidget()
//{

//}

void vtkBoundingBoxManipulatorWidget::PlaceWidget()
{
  double pts[8][4] = {
    {-0.5, -0.5, -0.5, 1.0},
    { 0.5, -0.5, -0.5, 1.0},
    { 0.5,  0.5, -0.5, 1.0},
    {-0.5,  0.5, -0.5, 1.0},
    {-0.5, -0.5,  0.5, 1.0},
    { 0.5, -0.5,  0.5, 1.0},
    { 0.5,  0.5,  0.5, 1.0},
    {-0.5,  0.5,  0.5, 1.0}
  };
  auto* m = this->Prop3D->GetUserMatrix();
  if (m)
  {
    for (int i = 0; i < 8; ++i)
    {
      m->MultiplyPoint(pts[i], pts[i]);
      pts[i][0] /= pts[i][3];
      pts[i][1] /= pts[i][3];
      pts[i][2] /= pts[i][3];
    }
  }
  // Set the points
  for (int i = 0; i < 8; ++i)
  {
    this->Points->SetPoint(i, pts[i][0], pts[i][1], pts[i][2]);
  }
  this->PositionHandles();
  this->ComputeNormals();
  this->SizeHandles();
}


void vtkBoundingBoxManipulatorWidget::GetTransform(vtkTransform* vtkNotUsed(t))
{
  double *pts = static_cast<vtkDoubleArray *>(this->Points->GetData())->GetPointer(0);
  double *p0 = pts;
  double *p1 = pts + 3*1;
  double *p3 = pts + 3*3;
  double *p4 = pts + 3*4;
  double *p14 = pts + 3*14;

  // Reset to Identity
  userMatrix->Identity();

  // Set translation
  userMatrix->SetElement(0, 3, p14[0]);
  userMatrix->SetElement(1, 3, p14[1]);
  userMatrix->SetElement(2, 3, p14[2]);

  // Set orientation
  this->PositionHandles();
  this->ComputeNormals();
  for (int i = 0; i < 3; i++)
  {
    userMatrix->SetElement(i,0,this->N[1][i]);
    userMatrix->SetElement(i,1,this->N[3][i]);
    userMatrix->SetElement(i,2,this->N[5][i]);
  }

  // Set scale
  double scaleX = 0.0, scaleY = 0.0, scaleZ = 0.0;
  for (int i = 0; i < 3; i++)
  {
    scaleX += std::pow(p1[i] - p0[i], 2);
    scaleY += std::pow(p3[i] - p0[i], 2);
    scaleZ += std::pow(p4[i] - p0[i], 2);
  }
  if (scaleX > 0)
    scaleX /= std::sqrt(scaleX);
  if (scaleY > 0)
    scaleY /= std::sqrt(scaleY);
  if (scaleZ > 0)
    scaleZ /= std::sqrt(scaleZ);

  for (int i = 0; i < 3; ++i)
  {
    userMatrix->SetElement(i, 0, userMatrix->GetElement(i, 0) * scaleX);
    userMatrix->SetElement(i, 1, userMatrix->GetElement(i, 1) * scaleY);
    userMatrix->SetElement(i, 2, userMatrix->GetElement(i, 2) * scaleZ);
  }
}
