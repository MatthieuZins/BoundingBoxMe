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

vtkStandardNewMacro(vtkBoundingBoxManipulatorWidget);

vtkBoundingBoxManipulatorWidget::vtkBoundingBoxManipulatorWidget()
  : prevUserTransform(vtkSmartPointer<vtkMatrix4x4>::New()),
    initialOrientation(vtkSmartPointer<vtkMatrix4x4>::New())
{

}

//vtkBoundingBoxManipulatorWidget::~vtkBoundingBoxManipulatorWidget()
//{

//}

void vtkBoundingBoxManipulatorWidget::PlaceWidget()
{

  auto *userMat = Prop3D->GetUserMatrix();
  if (userMat)
  {
    prevUserTransform->DeepCopy(userMat);

    double vx=0, vy=0, vz=0;
    for (int i = 0; i<3;++i)
    {
      vx += std::pow(userMat->GetElement(i,0), 2);
      vy += std::pow(userMat->GetElement(i,1), 2);
      vz += std::pow(userMat->GetElement(i,2), 2);
    }
    vx = std::sqrt(vx);
    vy = std::sqrt(vy);
    vz = std::sqrt(vz);
    std::cout << "scale from previous user transfrom = " << vx << " " << vy << " " << vz << std::endl;
  }

  double pt0[] = {-0.5, -0.5, -0.5, 1};
  double pt1[] = {-0.5, -0.5, 0.5 , 1};
  double pt2[] = {-0.5, 0.5, -0.5 , 1};
  double pt3[] = {-0.5, 0.5, 0.5  , 1};
  double pt4[] = {0.5, -0.5, -0.5 , 1};
  double pt5[] = {0.5, -0.5, 0.5  , 1};
  double pt6[] = {0.5, 0.5, -0.5  , 1};
  double pt7[] = {0.5, 0.5, 0.5   , 1};

  double pt0_out[4];
  double pt1_out[4];
  double pt2_out[4];
  double pt3_out[4];
  double pt4_out[4];
  double pt5_out[4];
  double pt6_out[4];
  double pt7_out[4];
  this->Prop3D->ComputeMatrix();
  vtkMatrix4x4* m = this->Prop3D->GetMatrix();

  std::cout << "matrix of actro\n" << *m << std::endl;
  m->MultiplyPoint(pt0, pt0_out);
  m->MultiplyPoint(pt1, pt1_out);
  m->MultiplyPoint(pt2, pt2_out);
  m->MultiplyPoint(pt3, pt3_out);
  m->MultiplyPoint(pt4, pt4_out);
  m->MultiplyPoint(pt5, pt5_out);
  m->MultiplyPoint(pt6, pt6_out);
  m->MultiplyPoint(pt7, pt7_out);



////  int i;
  double bounds[6], center[3];

////  this->AdjustBounds(bds,bounds,center);

  this->Points->SetPoint(0, pt0_out[0] / pt0_out[3], pt0_out[1] / pt0_out[3], pt0_out[2] / pt0_out[3]);
  this->Points->SetPoint(4, pt1_out[0] / pt1_out[3], pt1_out[1] / pt1_out[3], pt1_out[2] / pt1_out[3]);
  this->Points->SetPoint(3, pt2_out[0] / pt2_out[3], pt2_out[1] / pt2_out[3], pt2_out[2] / pt2_out[3]);
  this->Points->SetPoint(7, pt3_out[0] / pt3_out[3], pt3_out[1] / pt3_out[3], pt3_out[2] / pt3_out[3]);
  this->Points->SetPoint(1, pt4_out[0] / pt4_out[3], pt4_out[1] / pt4_out[3], pt4_out[2] / pt4_out[3]);
  this->Points->SetPoint(5, pt5_out[0] / pt5_out[3], pt5_out[1] / pt5_out[3], pt5_out[2] / pt5_out[3]);
  this->Points->SetPoint(2, pt6_out[0] / pt6_out[3], pt6_out[1] / pt6_out[3], pt6_out[2] / pt6_out[3]);
  this->Points->SetPoint(6, pt7_out[0] / pt7_out[3], pt7_out[1] / pt7_out[3], pt7_out[2] / pt7_out[3]);

  std::cout << pt0_out[0] / pt0_out[3] << " " << pt0_out[1] / pt0_out[3] << " " << pt0_out[2] / pt0_out[3] << std::endl;
  std::cout << pt1_out[0] / pt1_out[3] << " " << pt1_out[1] / pt1_out[3] << " " << pt1_out[2] / pt1_out[3] << std::endl;
  std::cout << pt2_out[0] / pt2_out[3] << " " << pt2_out[1] / pt2_out[3] << " " << pt2_out[2] / pt2_out[3] << std::endl;
  std::cout << pt3_out[0] / pt3_out[3] << " " << pt3_out[1] / pt3_out[3] << " " << pt3_out[2] / pt3_out[3] << std::endl;
  std::cout << pt4_out[0] / pt4_out[3] << " " << pt4_out[1] / pt4_out[3] << " " << pt4_out[2] / pt4_out[3] << std::endl;
  std::cout << pt5_out[0] / pt5_out[3] << " " << pt5_out[1] / pt5_out[3] << " " << pt5_out[2] / pt5_out[3] << std::endl;
  std::cout << pt6_out[0] / pt6_out[3] << " " << pt6_out[1] / pt6_out[3] << " " << pt6_out[2] / pt6_out[3] << std::endl;
  std::cout << pt7_out[0] / pt7_out[3] << " " << pt7_out[1] / pt7_out[3] << " " << pt7_out[2] / pt7_out[3] << std::endl;

  bounds[0] = -0.5;
  bounds[1] =  0.5;
  bounds[2] = -0.5;
  bounds[3] =  0.5;
  bounds[4] = -0.5;
  bounds[5] =  0.5;
  for (int i=0; i<6; i++)
  {
    this->InitialBounds[i] = bounds[i];
  }
  this->InitialLength = sqrt((bounds[1]-bounds[0])*(bounds[1]-bounds[0]) +
                             (bounds[3]-bounds[2])*(bounds[3]-bounds[2]) +
                             (bounds[5]-bounds[4])*(bounds[5]-bounds[4]));
  this->InitialCenter[0] = m->GetElement(0, 3);
  this->InitialCenter[1] = m->GetElement(1, 3);
  this->InitialCenter[2] = m->GetElement(2, 3);

  this->initialOrientation->DeepCopy(m);
  // need to remove translation and scaling
  this->initialOrientation->SetElement(0, 3, 0);
  this->initialOrientation->SetElement(1, 3, 0);
  this->initialOrientation->SetElement(2, 3, 0);
  double norme_vx = 0, norme_vy = 0, norme_vz = 0;
  for (int i = 0; i < 3; ++i)
  {
    norme_vx += std::pow(initialOrientation->GetElement(i, 0), 2);
    norme_vy += std::pow(initialOrientation->GetElement(i, 1), 2);
    norme_vz += std::pow(initialOrientation->GetElement(i, 2), 2);
  }
  norme_vx = std::sqrt(norme_vx);
  norme_vy = std::sqrt(norme_vy);
  norme_vz = std::sqrt(norme_vz);
  for (int i = 0; i < 3; ++i)
  {
    initialOrientation->SetElement(i, 0, initialOrientation->GetElement(i, 0) / norme_vx);
    initialOrientation->SetElement(i, 1, initialOrientation->GetElement(i, 1) / norme_vy);
    initialOrientation->SetElement(i, 2, initialOrientation->GetElement(i, 2) / norme_vz);
  }

  double scaleVec[3][3];

  for (int i=0; i<3; i++)
  {
    scaleVec[0][i] = (pt4_out[i] - pt0_out[i]);
    scaleVec[1][i] = (pt2_out[i] - pt0_out[i]);
    scaleVec[2][i] = (pt1_out[i] - pt0_out[i]);
  }

  this->InitialScale[0] = vtkMath::Norm(scaleVec[0]);
  this->InitialScale[1] = vtkMath::Norm(scaleVec[1]);
  this->InitialScale[2] = vtkMath::Norm(scaleVec[2]);

  std::cout << "Initila scale = " << InitialScale[0] << " " << InitialScale[1] << " " << InitialScale[2] << std::endl;

  this->PositionHandles();
  this->ComputeNormals();
  this->SizeHandles();
}


void vtkBoundingBoxManipulatorWidget::GetTransform(vtkTransform *t)
{
  double *pts =
         static_cast<vtkDoubleArray *>(this->Points->GetData())->GetPointer(0);
  double *p0 = pts;
  double *p1 = pts + 3*1;
  double *p3 = pts + 3*3;
  double *p4 = pts + 3*4;
  double *p14 = pts + 3*14;
  double center[3], translate[3], scale[3], scaleVec[3][3];
  int i;

  // The transformation is relative to the initial bounds.
  // Initial bounds are set when PlaceWidget() is invoked.
  t->Identity();
  t->PreMultiply();

  // Translation
  for (i=0; i<3; i++)
  {
    center[i] = p14[i] - InitialCenter[i];
  }
  translate[0] = center[0] + InitialCenter[0];
  translate[1] = center[1] + InitialCenter[1];
  translate[2] = center[2] + InitialCenter[2];
  t->Translate(translate[0], translate[1], translate[2]);

  // Orientation
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  this->PositionHandles();
  this->ComputeNormals();
  double vx = 0.0, vy = 0.0, vz = 0.0;
  for (i=0; i<3; i++)
  {
    matrix->SetElement(i,0,this->N[1][i]);
    matrix->SetElement(i,1,this->N[3][i]);
    matrix->SetElement(i,2,this->N[5][i]);
    vx += std::pow(matrix->GetElement(i, 0), 2);
    vy += std::pow(matrix->GetElement(i, 1), 2);
    vz += std::pow(matrix->GetElement(i, 2), 2);
  }
  vx /= std::sqrt(vx);
  vy /= std::sqrt(vy);
  vz /= std::sqrt(vz);
  for (i=0; i<3; i++)
  {
    matrix->SetElement(i,0,matrix->GetElement(i, 0) / vx);
    matrix->SetElement(i,1,matrix->GetElement(i, 1) / vy);
    matrix->SetElement(i,2,matrix->GetElement(i, 2) / vz);
  }
  auto initialOrientationInverse = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkMatrix4x4::Invert(initialOrientation, initialOrientationInverse);
  std::cout << "initial orientation \n" << *initialOrientation << "\n";
  std::cout << "initia orientation inverse \n" << *initialOrientationInverse << "\n";
  std::cout << "matrix from normals \n" << *matrix << "\n";
  t->Concatenate(matrix);
  t->Concatenate(initialOrientationInverse);
//  matrix->Delete();

  // Scale
  for (i=0; i<3; i++)
  {
    scaleVec[0][i] = (p1[i] - p0[i]);
    scaleVec[1][i] = (p3[i] - p0[i]);
    scaleVec[2][i] = (p4[i] - p0[i]);
  }


  scale[0] = vtkMath::Norm(scaleVec[0]);
//  if (this->InitialBounds[1] != this->InitialBounds[0])
  {
    scale[0] = scale[0] / this->InitialScale[0]; // (this->InitialBounds[1]-this->InitialBounds[0]);
  }
  scale[1] = vtkMath::Norm(scaleVec[1]);
//  if (this->InitialBounds[3] != this->InitialBounds[2])
  {
    scale[1] = scale[1] / this->InitialScale[1]; //(this->InitialBounds[3]-this->InitialBounds[2]);
  }
  scale[2] = vtkMath::Norm(scaleVec[2]);
//  if (this->InitialBounds[5] != this->InitialBounds[4])
  {
    scale[2] = scale[2] / this->InitialScale[2]; //(this->InitialBounds[5]-this->InitialBounds[4]);
  }
  t->Scale(scale[0],scale[1],scale[2]);
  std::cout << "adjusted scale = " << scale[0] << " " << scale[1] << " " << scale[2] << std::endl;

  // Add back in the contribution due to non-origin center
  t->Translate(-InitialCenter[0], -InitialCenter[1], -InitialCenter[2]);
}
