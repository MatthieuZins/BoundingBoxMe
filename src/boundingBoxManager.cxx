//=========================================================================
//
// Copyright 2019 Kitware, Inc.
// Author: Matthieu Zins
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "boundingBoxManager.h"

#include <QDebug>

#include "timeStepsManager.h"


std::unique_ptr<BoundingBoxManager> BoundingBoxManager::m_instance = nullptr;


BoundingBoxManager& BoundingBoxManager::getInstance()
{
  if (!m_instance)
    m_instance.reset(new BoundingBoxManager);
  return *m_instance;
}

void BoundingBoxManager::releaseInstance()
{
  if (m_instance)
    m_instance.reset();
}


/// This function try to add a new bounding box
/// if the instance id is already existing, then we add the pose and the frames interval
/// otherwaise we create it
/// the pose correspond to the full interval
const BoundingBox* BoundingBoxManager::addBoundingBox(BoundingBox::Id id,
                                                      const std::string &classe,
                                                      const Eigen::Isometry3d &pose,
                                                      const Eigen::Vector3d &dimension,
                                                      const std::pair<int, int>& framesInterval,
                                                      BoundingBox::State state)
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
    std::cout << "Add existing BBox!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
    // storing id does not change
    // instance id does not change
    // add the new frames and poses
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
    auto new_bb = std::make_shared<BoundingBox>(storing_id, id, classe, dimension, state);
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

std::vector<BoundingBox::Id> BoundingBoxManager::findBoundingBoxesPresentInFrame(unsigned int frameId) const
{
  std::vector<BoundingBox::Id> res;
  for (BoundingBox::Id i = 0; i < m_bbs.size(); ++i)
  {
    if (m_bbs[i]->isPresentInInterval(frameId, frameId))
    {
      res.push_back(i);
    }
  }
  return res;
}


