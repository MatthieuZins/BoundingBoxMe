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

Eigen::Isometry3d eigenIsometry3dFromVtkMatrix4x4(vtkMatrix4x4 *matrix, Eigen::Vector3d *scaling)
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
  Eigen::Vector3d v1 = iso.block<3, 1>(0, 0);
  Eigen::Vector3d v2 = iso.block<3, 1>(0, 1);
  Eigen::Vector3d v3 = iso.block<3, 1>(0, 2);
  double n1 = std::sqrt(v1.dot(v1));
  double n2 = std::sqrt(v2.dot(v2));
  double n3 = std::sqrt(v3.dot(v3));
  for (int i = 0; i < 3; ++i)
  {
    iso(i, 0) /= n1;
    iso(i, 1) /= n2;
    iso(i, 2) /= n3;
  }
  if (scaling)
  {
    (*scaling)[0] = n1;
    (*scaling)[1] = n2;
    (*scaling)[2] = n3;
  }
  return Eigen::Isometry3d(iso);
}
