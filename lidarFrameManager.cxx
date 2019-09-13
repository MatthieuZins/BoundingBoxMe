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

#include "lidarFrameManager.h"

#include <memory>

std::unique_ptr<LidarFrameManager> LidarFrameManager::m_instance = nullptr;

LidarFrameManager& LidarFrameManager::getInstance() {
  if (!m_instance)
    m_instance.reset(new LidarFrameManager);
  return *m_instance;
}

void LidarFrameManager::releaseInstance()
{
  if (m_instance)
    m_instance.reset();
}

void LidarFrameManager::addFrame(const LidarFrame &frame)
{
  m_frames.emplace_back(frame);
}

std::vector<vtkPolyData*> LidarFrameManager::getFramesPoints() const
{
  std::vector<vtkPolyData*> res;
  for (auto& f : m_frames)
  {
    res.push_back(f.m_points.Get());
  }
  return res;
}
