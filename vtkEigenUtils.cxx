#include "vtkEigenUtils.h"



vtkSmartPointer<vtkMatrix4x4> eigenIsometry3dToVtkMatrix4x4(const Eigen::Isometry3d &isometry)
{
  auto matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  const Eigen::Matrix4d& poseMat = isometry.matrix();
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      matrix->SetElement(i, j, poseMat(i, j));
    }
  }
  return matrix;
}

Eigen::Isometry3d eigenIsometry3dFromVtkMatrix4x4(vtkMatrix4x4 *matrix)
{
  // here we assume matrix represents an isometry
  Eigen::Matrix4d iso;
  for (int i = 0; i < 4; ++i)
  {
    for (int j = 0; j < 4; ++j)
    {
      iso(i, j) = matrix->GetElement(i, j);
    }
  }
  return Eigen::Isometry3d(iso);
}
