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

#ifndef TIMESTEPSMANAGER_H
#define TIMESTEPSMANAGER_H

#include <memory>

class TimeStepsManager
{

public:
  static TimeStepsManager& getInstance(size_t size=0);
  static void releaseInstance();
  ~TimeStepsManager() = default;

  void initializeSize(size_t size);

  void setModeAll();

  void setModeSingle(int index);

  void setModeInterval(int first, int last);

  std::pair<int, int> getCurrentTimeInterval() const {
    return std::make_pair(m_first, m_last);
  }

  bool isModeSingle() const {
    return m_first == m_last;
  }

  bool isModeAll() const {
    return m_last - m_first + 1 == m_size;
  }

private:
  static std::unique_ptr<TimeStepsManager> m_instance;

  TimeStepsManager() = default;
  TimeStepsManager(const TimeStepsManager&) = delete;
  TimeStepsManager& operator =(const TimeStepsManager&) = delete;

  int m_size;
  int m_first = 0, m_last = 0;
};

#endif // TIMESTEPMANAGER_H
