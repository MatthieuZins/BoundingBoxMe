#include "boundingBoxManager.h"

#include <memory>

#include <QDebug>

#include "timeStepsManager.h"

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
const BoundingBox* BoundingBoxManager::addBoundingBox(BoundingBox::Id id,
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

    // if we add a new observation of a bounding box, we consider it is moving
    existingBoundingBox->setState(BoundingBox::State::DYNAMIC);

    return existingBoundingBox;
  }
  else
  {
    BoundingBox::Id storing_id = m_bbs.size();
    BoundingBox::State state = BoundingBox::State::DYNAMIC;
    if (TimeStepsManager::getInstance().isModeAll())
      state = BoundingBox::State::STATIC;
    auto new_bb = std::make_shared<BoundingBox>(storing_id, id, classe, state);
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

BoundingBox *BoundingBoxManager::getBoundingBoxFromInstanceId(BoundingBox::Id id)
{
  for(auto& bbPtr : m_bbs)
  {
    if (bbPtr->getInstanceId() == id)
      return bbPtr.get();
  }
  return nullptr;
}

// This function deletes the presence in framesInterval of the bounding boxes stored at index
// if the bounding is not more present in any frames it gets completely remove
// The function returns if the bounding box is completely deleted
bool BoundingBoxManager::deleteBoundingBox(unsigned int index, const std::pair<int, int> &framesInterval)
{
  if (index >= 0 && index < m_bbs.size())
  {
    for (int f = framesInterval.first; f <= framesInterval.second; ++f)
    {
      m_bbs[index]->removePresenceInFrame(f);
    }

    // if the bounding box is not present anymore, we remove it completely
    if (!m_bbs[index]->hasAnyPresence())
    {
      // update the storing id of all the bbs that are stored after the bb which is deleted
      for (int i = index +1 ; i < m_bbs.size(); ++i)
      {
        m_bbs[i]->setStoringId(m_bbs[i]->getStoringId() - 1);
      }
      m_bbs.erase(m_bbs.begin() + index);
      return true;
    }
  }
  return false;
}


