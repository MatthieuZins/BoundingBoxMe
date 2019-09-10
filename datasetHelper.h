#ifndef DATASETHELPER_H
#define DATASETHELPER_H
#include <filesystem>
#include <iostream>

#include <yaml-cpp/yaml.h>
#include <Eigen/Dense>

namespace fs = std::filesystem;

std::vector<std::pair<std::string, double> > loadSeriesFile(const std::string& filename);

std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>> loadPosesFile(const std::string& filename);

bool loadLidarDataSet(const std::string& filename);


/// This function writes a bounding box dataset
void writeBBoxDataSet(const std::string& filename);

bool loadBBoxDataSet(const std::string& filename);

#endif // DATASETHELPER_H
