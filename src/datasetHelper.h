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

#ifndef DATASETHELPER_H
#define DATASETHELPER_H
#include <experimental/filesystem>
#include <iostream>

#include <yaml-cpp/yaml.h>
#include <Eigen/Dense>

namespace fs = std::experimental::filesystem;

std::vector<std::pair<std::string, double> > loadSeriesFile(const std::string& filename);

std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>> loadPosesFile(const std::string& filename);

bool loadLidarDataSet(const std::string& filename);


/// This function writes a bounding box dataset
void writeBBoxDataSet(const std::string& filename);

bool loadBBoxDataSet(const std::string& filename);

bool loadCameraDataSet(const std::string& filename);

#endif // DATASETHELPER_H
