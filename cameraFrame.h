#ifndef CAMERAFRAME_H
#define CAMERAFRAME_H

#include <string>

struct CameraFrame
{
  CameraFrame(const std::string& file, double time)
    : filename(file), timestamp(time)
  {}

  std::string filename;
  double timestamp;
};

#endif // CAMERAFRAME_H
