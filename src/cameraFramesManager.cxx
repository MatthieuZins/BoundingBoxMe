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

