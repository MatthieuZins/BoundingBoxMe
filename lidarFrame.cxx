#include "lidarFrame.h"

LidarFrame::LidarFrame(size_t id, vtkSmartPointer<vtkPolyData> points,
                       const Eigen::Isometry3d &pose, double time)
  : m_id(id), m_points(vtkSmartPointer<vtkPolyData>::New()), m_pose(pose), m_timestamp(time)
{
  m_points->DeepCopy(points);
}
