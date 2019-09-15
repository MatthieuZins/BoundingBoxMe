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

#include "boundingBox.h"

#include <QDebug>

BoundingBox::BoundingBox(BoundingBox::Id storingId, BoundingBox::Id instanceId,
                         const std::string &classe, const Eigen::Vector3d &dimensions, State state)
  : m_storingId(storingId), m_instanceId(instanceId),
    m_class(classe), m_dimensions(dimensions), m_state(state)
{
  if (m_dimensions.x() < 1e-3 && m_dimensions.x() < 1e-3 && m_dimensions.x() < 1e-3)
  {
    qWarning() << "Too small BBox. Dimension reset";
    m_dimensions.x() = 1.0;
    m_dimensions.y() = 1.0;
    m_dimensions.z() = 1.0;
  }
}



void BoundingBox::addPresenceInFrame(const Eigen::Isometry3d& pose, unsigned int frameId)
{
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
  if (it != m_framesIds.end())
  {
    return m_poses[std::distance(m_framesIds.begin(), it)];
  }
  else
  {
    qWarning() << "Try to get the pose in a frame where the bounding box is not present: " << frameId;
    return Eigen::Isometry3d::Identity();
  }
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


bool BoundingBox::isPresentInInterval(int first, int last) const
{
  for (auto& f : m_framesIds)
  {
    if (f >= first && f <= last)
      return true;
  }
  return false;
}

int BoundingBox::getFirstFrameOfPresenceInInterval(int first, int last) const
{
  int min = last;
  bool found = false;
  for (auto& f : m_framesIds)
  {
    if (f >= first && f <= last)
    {
      if (f < min)
        min = f;
      found = true;
    }
  }
  if (!found)
    return -1;
  return min;
}

