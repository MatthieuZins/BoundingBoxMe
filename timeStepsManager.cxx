#include "timeStepsManager.h"

std::unique_ptr<TimeStepsManager> TimeStepsManager::m_instance = nullptr;

TimeStepsManager &TimeStepsManager::getInstance(int size)
{
  if (!m_instance)
    m_instance.reset(new TimeStepsManager());
  m_instance->setSize(size);
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
  m_first = first;
  m_last = last;
}

std::pair<int, int> TimeStepsManager::getTimeStepsInterval() const
{
  return std::make_pair(m_first, m_last);
}

void TimeStepsManager::setSize(size_t size)
{
  m_size = static_cast<int>(size);
}
