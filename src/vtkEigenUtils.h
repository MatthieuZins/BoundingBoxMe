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

#ifndef VTKEIGENUTILS_H
#define VTKEIGENUTILS_H

#include <Eigen/Dense>
#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

vtkSmartPointer<vtkMatrix4x4> eigenIsometry3dToVtkMatrix4x4(const Eigen::Isometry3d& isometry);

Eigen::Isometry3d eigenIsometry3dFromVtkMatrix4x4(vtkMatrix4x4* matrix, Eigen::Vector3d* scaling=nullptr);

#endif // VTKEIGENUTILS_H
