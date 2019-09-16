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

#include "timeStepsManager.h"

#include <QDebug>

#include <iostream>

std::unique_ptr<TimeStepsManager> TimeStepsManager::m_instance = nullptr;

TimeStepsManager &TimeStepsManager::getInstance(size_t size)
{
  if (!m_instance)
    m_instance.reset(new TimeStepsManager());
  if (size > 0)
    m_instance->initializeSize(size);
  return *m_instance;
}

void TimeStepsManager::releaseInstance()
{
  if (m_instance)
    m_instance.reset();
}

void TimeStepsManager::setModeAll()
{
  m_first = 0;
  m_last = m_size - 1;
}

void TimeStepsManager::setModeSingle(int index)
{
  index = std::min(m_size-1, std::max(0, index));
  m_first = index;
  m_last = index;
}

void TimeStepsManager::setModeInterval(int first, int last)
{
  if (last < first)
  {
      qWarning() << "Wrong time steps interval";
  }
  first = std::max(0, std::min(m_size - 1, first));
  last = std::max(0, std::min(m_size - 1, last));
  m_first = first;
  m_last = last;
}


void TimeStepsManager::initializeSize(size_t size)
{
  m_size = static_cast<int>(size);
}
