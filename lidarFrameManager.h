#ifndef LIDARFRAMEMANAGER_H
#define LIDARFRAMEMANAGER_H

#include "lidarFrame.h"

class LidarFrameManager
{

public:
  static LidarFrameManager& getInstance();
  ~LidarFrameManager() = default;

  void addFrame(const LidarFrame& frame);

  std::vector<vtkPolyData*> getFramesPoints() const;

  Eigen::Isometry3d getFramePose(unsigned int frameId) const;

  double getFrameTimestamp(unsigned int frameId) const;

  unsigned int getNbFrames() const {
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
