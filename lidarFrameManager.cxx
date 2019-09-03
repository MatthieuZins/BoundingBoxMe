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
  double bounds[6];
  for (auto& f : m_frames)
  {
    res.push_back(f.m_points.Get());
    std::cout << *f.m_points << "\n";
//    f.m_points->GetPoints()->GetBounds(bounds);
//    std::cout << bounds[0] << " " << bounds[1] << " " << bounds[2] << " "
//              << bounds[3] << " " << bounds[4] << " " << bounds[5] << "\n";
  }
  return res;
}
