#ifndef LIDARFRAME_H
#define LIDARFRAME_H

#include <vtkPolyData.h>
#include <Eigen/Dense>

struct LidarFrame
{
  LidarFrame(int id, vtkSmartPointer<vtkPolyData> points,
             const Eigen::Isometry3d& pose=Eigen::Isometry3d::Identity(), double time=0.0);

  int m_id;
  vtkSmartPointer<vtkPolyData> m_points;
  Eigen::Isometry3d m_pose;
  double m_timestamp;


  EIGEN_MAKE_ALIGNED_OPERATOR_NEW;

};

#endif // LIDARFRAME_H
