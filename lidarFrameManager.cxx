#include "lidarFrameManager.h"

#include <memory>

std::unique_ptr<LidarFrameManager> LidarFrameManager::m_instance = nullptr;

LidarFrameManager& LidarFrameManager::getInstance() {
  if (!m_instance)
    m_instance.reset(new LidarFrameManager);
  return *m_instance;
}

void LidarFrameManager::addFrame(const LidarFrame &frame)
{
  m_frames.emplace_back(frame);
}

std::vector<vtkPolyData*> LidarFrameManager::getFramesPoints() const
{
  std::vector<vtkPolyData*> res;
  for (auto& f : m_frames)
    res.push_back(f.m_points.Get());
  return res;
}
