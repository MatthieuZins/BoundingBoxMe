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

vtkSmartPointer<vtkMatrix4x4> BoundingBox::getPoseVtkMatrix(unsigned int frameId) const
{
  auto it = std::find(m_framesIds.begin(), m_framesIds.end(), frameId);
  auto mat = vtkSmartPointer<vtkMatrix4x4>::New();
  mat->Identity();
  if (it != m_framesIds.end())
  {
    int index = std::distance(m_framesIds.begin(), it);
    const Eigen::Matrix4d& poseMat = m_poses[index].matrix();
    for (int i = 0; i < 4; ++i)
    {
      for (int j = 0; j < 4; ++j)
      {
        mat->SetElement(i, j, poseMat(i, j));
      }
    }
  }
  else
  {
    qWarning() << "Bounding Box not present in frame: " << frameId << "(BoundingBox::getPoseVtkMatrix)";
  }
  return mat;
}
