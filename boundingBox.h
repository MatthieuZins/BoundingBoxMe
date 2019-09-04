#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <Eigen/Dense>

class BoundingBox
{
public:
  using  Id = unsigned long;

  BoundingBox() = default;

private:

  Id m_storingId;
  Id m_instanceId;

  // global (constant over the whole set of frames)
  Eigen::Vector3d m_dimensions;
  std::string m_class;


  std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>> m_poses;
  std::vector<unsigned int> m_frameIds;


  EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#endif // BOUNDINGBOX_H
