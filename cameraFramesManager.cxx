#include "cameraFramesManager.h"

std::unique_ptr<CameraFramesManager> CameraFramesManager::m_instance = nullptr;

CameraFramesManager &CameraFramesManager::getInstance()
{
  if (!m_instance)
    m_instance.reset(new CameraFramesManager);
  return *m_instance;
}

void CameraFramesManager::releaseInstance()
{
  if (m_instance)
    m_instance.reset();
}

void CameraFramesManager::addFrame(const std::string &filename, double time)
{
  m_frames.push_back(std::make_unique<CameraFrame>(filename, time));
}

std::string CameraFramesManager::getFrame(double time)
{
  if (m_frames.size() == 0)
    return "";
  int index = 0;
  while (index < m_frames.size() && m_frames[index]->timestamp < time)
    ++index;
  if (index > 0)
    --index;
  return m_frames[index]->filename;
}

