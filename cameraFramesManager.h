#ifndef CAMERAFRAMESMANAGER_H
#define CAMERAFRAMESMANAGER_H

#include <memory>
#include <vector>
#include <algorithm>

#include "cameraFrame.h"



class CameraFramesManager
{
public:
  static CameraFramesManager& getInstance();
  static void releaseInstance();
  ~CameraFramesManager() = default;

  void addFrame(const std::string& filename, double time);

  std::string getFrame(double time);

  void finalizeAddingFrames();

private:
  static std::unique_ptr<CameraFramesManager> m_instance;

  CameraFramesManager() = default;
  CameraFramesManager(const CameraFramesManager&) = delete;
  CameraFramesManager& operator = (const CameraFramesManager&) = delete;

  std::vector<std::unique_ptr<CameraFrame>> m_frames;


};

inline void CameraFramesManager::finalizeAddingFrames()
{
  std::sort(m_frames.begin(), m_frames.end(), [](std::unique_ptr<CameraFrame>& f1, std::unique_ptr<CameraFrame>& f2) {
    return f1->timestamp < f2->timestamp;
  });
}

#endif // CAMERAFRAMESMANAGER_H
