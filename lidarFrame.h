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

#ifndef LIDARFRAME_H
#define LIDARFRAME_H

#include <vtkPolyData.h>
#include <Eigen/Dense>

struct LidarFrame
{
  LidarFrame(size_t id, vtkSmartPointer<vtkPolyData> points,
             const Eigen::Isometry3d& pose=Eigen::Isometry3d::Identity(), double time=0.0);

  size_t m_id;
  vtkSmartPointer<vtkPolyData> m_points;
  Eigen::Isometry3d m_pose;
  double m_timestamp;


  EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

};

#endif // LIDARFRAME_H
