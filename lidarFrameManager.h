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

private:
  static std::unique_ptr<LidarFrameManager> m_instance;
  static bool m_flag;

  LidarFrameManager() = default;
  LidarFrameManager(const LidarFrameManager&) = delete;
  LidarFrameManager& operator =(const LidarFrameManager&) = delete;

  std::vector<LidarFrame> m_frames;
};

#endif // LIDARFRAMEMANAGER_H
