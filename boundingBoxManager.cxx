#include "boundingBoxManager.h"

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

std::shared_ptr<BoundingBox> BoundingBoxManager::appendBoundingBox(BoundingBox::Id id,
                                                                   const std::string &classe,
                                                                   const Eigen::Isometry3d &pose,
                                                                   const Eigen::Vector3d &dimension,
                                                                   int frame)
{
  BoundingBox::Id storing_id = m_bbs.size();
  auto bb = std::make_shared<BoundingBox>(storing_id, id, classe);

  m_bbs.push_back(bb);
  return bb;
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
