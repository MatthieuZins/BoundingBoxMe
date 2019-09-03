#ifndef TIMESTEPSMANAGER_H
#define TIMESTEPSMANAGER_H

#include <memory>

class TimeStepsManager
{

public:
  static TimeStepsManager& getInstance(int size);
  ~TimeStepsManager() = default;

  void setSize(size_t size);

  void setModeAll();

  void setModeSingle(int index);

  void setModeInterval(int first, int last);

  std::pair<int, int> getTimeStepsInterval() const;

private:
  static std::unique_ptr<TimeStepsManager> m_instance;

  TimeStepsManager() = default;
  TimeStepsManager(const TimeStepsManager&) = delete;
  TimeStepsManager& operator =(const TimeStepsManager&) = delete;

  int m_size;
  int m_first = 0, m_last = 0;
};

#endif // TIMESTEPMANAGER_H
