#include "boundingBoxManager.h"

#include <memory>

#include <QDebug>

std::unique_ptr<BoundingBoxManager> BoundingBoxManager::m_instance = nullptr;


BoundingBoxManager& BoundingBoxManager::getInstance()
{
  if (!m_instance)
    m_instance.reset(new BoundingBoxManager);
  return *m_instance;
}

void BoundingBoxManager::initializeClassesToHandle(const ClassesManager &classesManager)
{
  if (!m_initiliazed)
  {

    for (const auto& cl : classesManager.getAvailableClasses())
    {
      m_perClassCount[cl] = 0;
    }
  }
}

/// This function try to add a new bounding box
/// if the instance id is already existing, then we add the pose and the frames interval
/// otherwaise we create it
/// the pose correspond to the full interval
const BoundingBox* BoundingBoxManager::appendBoundingBox(BoundingBox::Id id,
                                                         const std::string &classe,
                                                         const Eigen::Isometry3d &pose,
                                                         const Eigen::Vector3d &dimension,
                                                         const std::pair<int, int>& framesInterval)
{
  BoundingBox* existingBoundingBox = nullptr;
  for (const auto& bb : m_bbs)
  {
    if (id ==  bb->getInstanceId())
    {
      existingBoundingBox = bb.get();
      break;
    }
  }

  if (existingBoundingBox)
  {
    // storing id does not change
    // instance id does not change
    // frames and poses
    for (int f = framesInterval.first; f <= framesInterval.second; ++f)
    {
      existingBoundingBox->addPresenceInFrame(pose, f);
    }

    // dimensions: force the new dimension
    existingBoundingBox->setDimensions(dimension);

    // the class does not change
    if (existingBoundingBox->getClass() != classe)
    {
      qWarning() << "The class (" << QString::fromStdString(classe) << ") does not match the initial class (" << QString::fromStdString(existingBoundingBox->getClass()) << ")";
    }
    return existingBoundingBox;
  }
  else
  {
    BoundingBox::Id storing_id = m_bbs.size();
    auto new_bb = std::make_shared<BoundingBox>(storing_id, id, classe);
    for (int f = framesInterval.first; f <= framesInterval.second; ++f)
    {
      new_bb->addPresenceInFrame(pose, f);
    }
    new_bb->setDimensions(dimension);
    m_bbs.push_back(new_bb);
    return new_bb.get();
  }
}

BoundingBox::Id BoundingBoxManager::findFirstUnusedInstanceId() const
{
  std::vector<int> counts(m_bbs.size(), 0);
  for (const auto& bb : m_bbs)
  {
    auto id = bb->getInstanceId();
    if (id < counts.size())
      ++counts[bb->getInstanceId()];
  }
  return std::distance(counts.begin(), std::find(counts.begin(), counts.end(), 0));
}
