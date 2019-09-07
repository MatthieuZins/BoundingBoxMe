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

std::pair<int, int> TimeStepsManager::getTimeStepsInterval() const
{
  return std::make_pair(m_first, m_last);
}

void TimeStepsManager::initializeSize(size_t size)
{
  m_size = static_cast<int>(size);
}
