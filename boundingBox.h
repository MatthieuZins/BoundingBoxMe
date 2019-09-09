#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <Eigen/Dense>

#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>

class BoundingBox
{
public:
  using  Id = unsigned long;

  BoundingBox(Id storingId, Id instanceId, const std::string& classe);

  bool isInFrame(unsigned int frameId) {
    return std::find(m_framesIds.begin(), m_framesIds.end(), frameId) != m_framesIds.end();
  }

  Eigen::Vector3d getCenter(unsigned int frameId) const;
  Eigen::Quaterniond getOrientation(unsigned int frameId) const;

  const std::vector<unsigned int>& getFrames() const {
    return m_framesIds;
  }

  const std::string& getClass() const {
    return m_class;
  }

  void setClass(const std::string& classe) {
    m_class = classe;
  }

  void addPresenceInFrame(const Eigen::Isometry3d& pose,  unsigned int frameId);
  void removePresenceInFrame(unsigned int frameId);
  bool hasPresence() const {
    return m_framesIds.size() > 0;
  }

  Eigen::Isometry3d getPose(unsigned int frameId) const;

  Id getInstanceId() const {
    return m_instanceId;
  }

  bool setPose(unsigned int frameId, const Eigen::Isometry3d &pose);

  Id getStoringId() const {
    return m_storingId;
  }

  Eigen::Vector3d getDimensions() const {
    return m_dimensions;
  }

  void setDimensions(const Eigen::Vector3d& dims) {
    m_dimensions = dims;
  }


private:

  Id m_storingId;
  Id m_instanceId;

  // global (constant over the whole set of frames)
  Eigen::Vector3d m_dimensions = Eigen::Vector3d(0.0, 0.0, 0.0);
  std::string m_class;


  std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>> m_poses;
  std::vector<unsigned int> m_framesIds;


  EIGEN_MAKE_ALIGNED_OPERATOR_NEW;
};

#endif // BOUNDINGBOX_H
