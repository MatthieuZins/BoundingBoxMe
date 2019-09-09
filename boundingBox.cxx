#include "boundingBox.h"

#include <QDebug>


BoundingBox::BoundingBox(BoundingBox::Id storingId, BoundingBox::Id instanceId,
                         const std::string &classe)
  : m_storingId(storingId), m_instanceId(instanceId),
    m_class(classe)
{

}

Eigen::Vector3d BoundingBox::getCenter(unsigned int frameId) const
{
  auto it = std::find(m_framesIds.begin(), m_framesIds.end(), frameId);
  if (it != m_framesIds.end())
  {
    int index = std::distance(m_framesIds.begin(), it);
    std::cout << m_poses[index].translation() << std::endl;
    return m_poses[index].translation();
  }
  else
  {
    qWarning() << "Bounding Box not present in frame: " << frameId << "(BoundingBox::getCenter)";
    return Eigen::Vector3d::Zero();
  }
}

Eigen::Quaterniond BoundingBox::getOrientation(unsigned int frameId) const
{
  auto it = std::find(m_framesIds.begin(), m_framesIds.end(), frameId);
  if (it != m_framesIds.end())
  {
    int index = std::distance(m_framesIds.begin(), it);
    return Eigen::Quaterniond(m_poses[index].rotation());
  }
  else
  {
    qWarning() << "Bounding Box not present in frame: " << frameId << "(BoundingBox::getOrientation)";
    return Eigen::Quaterniond::Identity();
  }
}

void BoundingBox::addPresenceInFrame(const Eigen::Isometry3d& pose, unsigned int frameId)
{
  //TODO maybe frame id should stay sorted
  m_framesIds.emplace_back(frameId);
  m_poses.emplace_back(pose);
}

void BoundingBox::removePresenceInFrame(unsigned int frameId)
{
  for (int i = 0; i < m_framesIds.size(); ++i)
  {
    if (frameId == m_framesIds[i])
    {
      m_framesIds.erase(m_framesIds.begin() + i);
      m_poses.erase(m_poses.begin() + i);
      break;
    }
  }
}

Eigen::Isometry3d BoundingBox::getPose(unsigned int frameId) const
{
  auto it = std::find(m_framesIds.begin(), m_framesIds.end(), frameId);
  auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
  mat->Identity();
  if (it != m_framesIds.end())
  {
    int index = std::distance(m_framesIds.begin(), it);
    return m_poses[index];
  }
  else
  {
    qWarning() << "Try to get the pose in a frame where the bounding box is not present: " << frameId;
  }
  return Eigen::Isometry3d::Identity();
}

bool BoundingBox::setPose(unsigned int frameId, const Eigen::Isometry3d& pose)
{
  for (int i = 0; i < m_framesIds.size(); ++i)
  {
    if (m_framesIds[i] == frameId)
    {
      m_poses[i] = pose;
      return true;
    }
  }
  qWarning() << "Try to set pose in a frame where the bounding box is not present: " << frameId;
  return false;
}
