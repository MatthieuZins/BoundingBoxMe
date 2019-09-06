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
  : prevUserTransform(vtkSmartPointer<vtkMatrix4x4>::New()),
    initialOrientation(vtkSmartPointer<vtkMatrix4x4>::New()),
    userMatrix(vtkSmartPointer<vtkMatrix4x4>::New())
{

}

//vtkBoundingBoxManipulatorWidget::~vtkBoundingBoxManipulatorWidget()
//{

//}

void vtkBoundingBoxManipulatorWidget::PlaceWidget()
{
  Prop3D->ComputeMatrix();
  std::copy(Prop3D->GetCenter(), Prop3D->GetCenter() + 3, actorCenter);
  std::copy(Prop3D->GetCenter(), Prop3D->GetCenter() + 3, actorOrigin);
  std::copy(Prop3D->GetOrientation(), Prop3D->GetOrientation() + 3, actorOrientation);
  std::copy(Prop3D->GetScale(), Prop3D->GetScale() + 3, actorScale);

  std::cout << "Place Widget :\n";
  std::cout << "center = " << Prop3D->GetCenter()[0] << " " << Prop3D->GetCenter()[1] << " " << Prop3D->GetCenter()[2] << std::endl;
  std::cout << "origin = " << Prop3D->GetOrigin()[0] << " " << Prop3D->GetOrigin()[1] << " " << Prop3D->GetOrigin()[2] << std::endl;
  std::cout << "orientation = " << Prop3D->GetOrientation()[0] << " " << Prop3D->GetOrientation()[1] << " " << Prop3D->GetOrientation()[2] << std::endl;
  std::cout << "scale = " << Prop3D->GetScale()[0] << " " << Prop3D->GetScale()[1] << " " << Prop3D->GetScale()[2] << std::endl;


  double pt0[] = {-0.5, -0.5, -0.5, 1.0};
  double pt1[] = {-0.5, -0.5,  0.5, 1.0};
  double pt2[] = {-0.5,  0.5, -0.5, 1.0};
  double pt3[] = {-0.5,  0.5,  0.5, 1.0};
  double pt4[] = { 0.5, -0.5, -0.5, 1.0};
  double pt5[] = { 0.5, -0.5,  0.5, 1.0};
  double pt6[] = { 0.5,  0.5, -0.5, 1.0};
  double pt7[] = { 0.5,  0.5,  0.5, 1.0};

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

  std::copy(p14, p14 + 3, actorCenter); // update position
  std::copy(actorCenter, actorCenter + 3, actorOrigin); // update position


  userMatrix->Identity();
  userMatrix->SetElement(0, 3, p14[0]);
  userMatrix->SetElement(1, 3, p14[1]);
  userMatrix->SetElement(2, 3, p14[2]);


  // The transformation is relative to the initial bounds.
  // Initial bounds are set when PlaceWidget() is invoked.
//  t->Identity();
//  t->PreMultiply();

  // Translation
//  for (i=0; i<3; i++)
//  {
//    center[i] = p14[i] - InitialCenter[i];
//  }
//  translate[0] = center[0] + InitialCenter[0];
//  translate[1] = center[1] + InitialCenter[1];
//  translate[2] = center[2] + InitialCenter[2];
//  t->Translate(translate[0], translate[1], translate[2]);

  // Orientation
//  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  Eigen::Matrix3d rot;

  this->PositionHandles();
  this->ComputeNormals();
//  double vx = 0.0, vy = 0.0, vz = 0.0;
  for (i=0; i<3; i++)
  {
//    matrix->SetElement(i,0,this->N[1][i]);
//    matrix->SetElement(i,1,this->N[3][i]);
//    matrix->SetElement(i,2,this->N[5][i]);
//    vx += std::pow(matrix->GetElement(i, 0), 2);
//    vy += std::pow(matrix->GetElement(i, 1), 2);
//    vz += std::pow(matrix->GetElement(i, 2), 2);
    rot(i, 0) = this->N[1][i];
    rot(i, 1) = this->N[3][i];
    rot(i, 2) = this->N[5][i];

    userMatrix->SetElement(i,0,this->N[1][i]);
    userMatrix->SetElement(i,1,this->N[3][i]);
    userMatrix->SetElement(i,2,this->N[5][i]);
  }

  std::cout << "rot \n " << rot << std::endl;

//  vx /= std::sqrt(vx);
//  vy /= std::sqrt(vy);
//  vz /= std::sqrt(vz);
//  for (i=0; i<3; i++)
//  {
//    matrix->SetElement(i,0,matrix->GetElement(i, 0) / vx);
//    matrix->SetElement(i,1,matrix->GetElement(i, 1) / vy);
//    matrix->SetElement(i,2,matrix->GetElement(i, 2) / vz);
//  }

  Eigen::EulerAnglesZXYd eulerAnglesRot(rot);
//  Eigen::EulerAnglesYXZd eulerAnglesRot(rot);

  double rotAroundZ = eulerAnglesRot.alpha();
  double rotAroundX = eulerAnglesRot.beta();
  double rotAroundY = eulerAnglesRot.gamma();

//  double rotAroundY = eulerAnglesRot.alpha();
//  double rotAroundX = eulerAnglesRot.beta();
//  double rotAroundZ = eulerAnglesRot.gamma();

  actorOrientation[0] = vtkMath::DegreesFromRadians(rotAroundX);
  actorOrientation[1] = vtkMath::DegreesFromRadians(rotAroundY);
  actorOrientation[2] = vtkMath::DegreesFromRadians(rotAroundZ);



//  auto initialOrientationInverse = vtkSmartPointer<vtkMatrix4x4>::New();
//  vtkMatrix4x4::Invert(initialOrientation, initialOrientationInverse);
//  std::cout << "initial orientation \n" << *initialOrientation << "\n";
//  std::cout << "initia orientation inverse \n" << *initialOrientationInverse << "\n";
//  std::cout << "matrix from normals \n" << *matrix << "\n";
//  t->Concatenate(matrix);
//  t->Concatenate(initialOrientationInverse);
//  matrix->Delete();

  // Scale
  for (i=0; i<3; i++)
  {
    scaleVec[0][i] = (p1[i] - p0[i]);
    scaleVec[1][i] = (p3[i] - p0[i]);
    scaleVec[2][i] = (p4[i] - p0[i]);
  }

  actorScale[0] = vtkMath::Norm(scaleVec[0]);
  actorScale[1] = vtkMath::Norm(scaleVec[1]);
  actorScale[2] = vtkMath::Norm(scaleVec[2]);

  for (int i = 0; i < 3; ++i)
  {
    userMatrix->SetElement(i, 0, userMatrix->GetElement(i, 0) * actorScale[0]);
    userMatrix->SetElement(i, 1, userMatrix->GetElement(i, 1) * actorScale[1]);
    userMatrix->SetElement(i, 2, userMatrix->GetElement(i, 2) * actorScale[2]);
  }

//  t->Scale(scale[0],scale[1],scale[2]);
//  std::cout << "adjusted scale = " << scale[0] << " " << scale[1] << " " << scale[2] << std::endl;

  // Add back in the contribution due to non-origin center
//  t->Translate(-InitialCenter[0], -InitialCenter[1], -InitialCenter[2]);


}
