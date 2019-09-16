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

#ifndef LIDARFRAMEMANAGER_H
#define LIDARFRAMEMANAGER_H

#include "lidarFrame.h"

class LidarFrameManager
{

public:
  static LidarFrameManager& getInstance();
  static void releaseInstance();
  ~LidarFrameManager() = default;

  void addFrame(const LidarFrame& frame);

  std::vector<vtkPolyData*> getFramesPoints() const;

  Eigen::Isometry3d getFramePose(unsigned int frameId) const;

  double getFrameTimestamp(unsigned int frameId) const;

  size_t getNbFrames() const {
    return m_frames.size();
  }

private:
  static std::unique_ptr<LidarFrameManager> m_instance;

  LidarFrameManager() = default;
  LidarFrameManager(const LidarFrameManager&) = delete;
  LidarFrameManager& operator =(const LidarFrameManager&) = delete;

  std::vector<LidarFrame> m_frames;
};

inline Eigen::Isometry3d LidarFrameManager::getFramePose(unsigned int frameId) const
{
  if (frameId >= 0 && frameId < m_frames.size())
    return m_frames[frameId].m_pose;
  return Eigen::Isometry3d::Identity();
}

inline double LidarFrameManager::getFrameTimestamp(unsigned int frameId) const
{
  if (frameId >= 0 && frameId < m_frames.size())
    return m_frames[frameId].m_timestamp;
  return 0.0;
}

#endif // LIDARFRAMEMANAGER_H
