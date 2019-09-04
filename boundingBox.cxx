#include "boundingBox.h"

#include <QDebug>


BoundingBox::BoundingBox(BoundingBox::Id storingId, BoundingBox::Id instanceId,
                         const std::string &classe)
  : m_storingId(storingId), m_instanceId(instanceId),
    m_class(classe)
{

}

Eigen::Vector3d BoundingBox::getCenter(unsigned int frameId)
{
  auto it = std::find(m_framesIds.begin(), m_framesIds.end(), frameId);
  if (it != m_framesIds.end())
  {
    int index = std::distance(m_framesIds.begin(), it);
    return m_poses[index].translation();
  }
  else
  {
    qWarning() << "Bounding Box not present in frame: " << frameId;
    return Eigen::Vector3d::Zero();
  }
}

void BoundingBox::addPresenceInFrame(const Eigen::Isometry3d& pose, unsigned int frameId)
{
  //TODO maybe frame id should stay sorted
  m_framesIds.emplace_back(frameId);
  m_poses.emplace_back(pose);
}
