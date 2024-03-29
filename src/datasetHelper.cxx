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

#include "datasetHelper.h"

#include <fstream>
#include <sstream>
#include <memory>

#include <QDebug>
#include <vtkXMLPolyDataReader.h>
#include <vtkSmartPointer.h>
#include "lidarFrameManager.h"
#include <vtkDelimitedTextReader.h>
#include <vtkTable.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtk_jsoncpp.h>

#include <unsupported/Eigen/EulerAngles>

#include "vtkEigenUtils.h"
#include "boundingBoxManager.h"
#include "boundingBox.h"
#include "timeStepsManager.h"
#include "lidarFrameManager.h"
#include "cameraFramesManager.h"
#include <unsupported/Eigen/EulerAngles>


namespace
{
vtkDoubleArray* foundArray(vtkTable* table, std::vector<std::string> potentialName)
{
  // try to find the right array
  for (const auto& name: potentialName)
  {
    vtkDoubleArray* tmp = vtkDoubleArray::SafeDownCast(table->GetColumnByName(name.c_str()));
    if (tmp)
    {
      return tmp;
    }
  }
  // throw an exception when no matching could be founded
  std::stringstream ss;
  ss << "Could not find of the following colum: ";
  for (const auto& name: potentialName)
  {
    ss << name << " , ";
  }
  throw std::runtime_error(ss.str());
}

std::unordered_map<std::string, vtkDoubleArray*> createArrayIndex(vtkTable* table)
{
  std::unordered_map<std::string, vtkDoubleArray*> array;
  std::vector<std::string> potentialName;

  std::string time[] = {"Time", "time", "Timestamp", "timestamp"};
  potentialName = std::vector<std::string>(time, time + sizeof(time) / sizeof(time[0]));
  array["time"] = foundArray(table, potentialName);

  std::string roll[] = {"Rx(Roll)", "Roll", "roll", "Rx", "rx"};
  potentialName = std::vector<std::string>(roll, roll + sizeof(roll) / sizeof(roll[0]));
  array["roll"] = foundArray(table, potentialName);

  std::string pitch[] = {"Ry(Pitch)", "Pitch", "pitch", "Ry", "ry"};
  potentialName = std::vector<std::string>(pitch, pitch + sizeof(pitch) / sizeof(pitch[0]));
  array["pitch"] = foundArray(table, potentialName);

  std::string yaw[] = {"Rz(Yaw)", "Yaw", "yaw", "Rz", "rz"};
  potentialName = std::vector<std::string>(yaw, yaw + sizeof(yaw) / sizeof(yaw[0]));
  array["yaw"] = foundArray(table, potentialName);

  std::string x[] = {"X", "x"};
  potentialName = std::vector<std::string>(x, x + sizeof(x) / sizeof(x[0]));
  array["X"] = foundArray(table, potentialName);

  std::string y[] = {"Y", "y"};
  potentialName = std::vector<std::string>(y, y + sizeof(y) / sizeof(y[0]));
  array["Y"] = foundArray(table, potentialName);

  std::string z[] = {"Z", "z"};
  potentialName = std::vector<std::string>(z, z + sizeof(z) / sizeof(z[0]));
  array["Z"] = foundArray(table, potentialName);

  return array;
}
}


std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d> > loadPosesFile(const std::string &filename)
{
  std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>> poses;
  std::ifstream file(filename);
  if (file.is_open())
  {
    // Read the data from the file
    auto csvReader = vtkSmartPointer<vtkDelimitedTextReader>::New();
    csvReader->SetFileName(filename.c_str());
    csvReader->DetectNumericColumnsOn();
    csvReader->SetFieldDelimiterCharacters(",");
    csvReader->ForceDoubleOn();
    csvReader->SetHaveHeaders(true);
    csvReader->Update();

    vtkTable* table = csvReader->GetOutput();

    if (table->GetNumberOfColumns() < 7)
    {
      qCritical() << "Invalid poses file: incorrect number of columns";
      return poses;
    }

    // create map for idx
    std::unordered_map<std::string, vtkDoubleArray*> array;
    try
    {
      array = createArrayIndex(table);
    }
    catch (std::exception& e)
    {
      qCritical() << "Invalid poses file: " << e.what();
      return poses;
    }

    // Read the data from the csv table
    for (vtkIdType i = 0; i < table->GetNumberOfRows(); i++)
    {
     Eigen::Translation3d t(array["X"]->GetTuple1(i), array["Y"]->GetTuple1(i), array["Z"]->GetTuple1(i));
     Eigen::EulerAnglesXYZd r(array["roll"]->GetTuple1(i), array["pitch"]->GetTuple1(i), array["yaw"]->GetTuple1(i));
     poses.emplace_back(t *  Eigen::Quaterniond(r.toRotationMatrix()));
    }
  }
  else
  {
    qCritical() << "Impossible to open the poses file: " << QString::fromStdString(filename);
  }
  return poses;
}

std::vector<std::pair<std::string, double> > loadSeriesFile(const std::string &filename)
{
  std::vector<std::pair<std::string, double>> dataset;
  try
  {
    YAML::Node node = YAML::LoadFile(filename);
    YAML::Node files = node["files"];
    fs::path directory = fs::path(filename).parent_path();

    for (size_t i = 0; i < files.size(); ++i)
    {
      fs::path file(files[i]["name"].as<std::string>());

      dataset.emplace_back(std::make_pair((directory / file).generic_string(), files[i]["time"].as<double>()));
    }
  }
  catch (std::exception& e)
  {
    qCritical() << "Error opening the dataset: " << QString::fromStdString(e.what());
  }
  return dataset;
}



bool loadLidarDataSet(const std::string &filename)
{
  fs::path path(filename);
  fs::path directory = path.parent_path();

  fs::path name = path.filename();
  fs::path posesFile = directory / name.replace_extension("poses");

  auto listOfFramesAndTimes = loadSeriesFile(filename);
  if (listOfFramesAndTimes.size() > 0)
  {
    auto poses = loadPosesFile(posesFile.generic_string());

    if (poses.size() != listOfFramesAndTimes.size())
    {
      qCritical() << "Erro opening the dataset: number of .vtp is different from number of poses";
      return false;
    }

    auto& lidarFramesManager = LidarFrameManager::getInstance();

    auto reader = vtkSmartPointer<vtkXMLPolyDataReader>::New();
    auto transformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    auto transform = vtkSmartPointer<vtkTransform>::New();

    for (size_t i = 0; i < listOfFramesAndTimes.size(); ++i)
    {
      reader->SetFileName(listOfFramesAndTimes[i].first.c_str());

      transform->SetMatrix(eigenIsometry3dToVtkMatrix4x4(poses[i]));
      transformFilter->SetInputConnection(reader->GetOutputPort());
      transformFilter->SetTransform(transform);
      transformFilter->Update();
      lidarFramesManager.addFrame({i, transformFilter->GetOutput(), poses[i], listOfFramesAndTimes[i].second});
    }
    return true;
  }
  return false;
}

void writeBBoxDataSet(const std::string &filename)
{
  fs::path path(filename);
  fs::path directory = path.parent_path();

  fs::path name = path.filename().replace_extension();

  auto& bboxManager = BoundingBoxManager::getInstance();
  auto& lidarFramesManager = LidarFrameManager::getInstance();
  const int N = lidarFramesManager.getNbFrames();

  Json::Value outputSeries;
  Json::Value files(Json::arrayValue);

  for (int frameId = 0; frameId < N; ++frameId)
  {
    Eigen::Isometry3d lidarFramePoseInv = lidarFramesManager.getFramePose(frameId).inverse();
    auto bbsPresent = bboxManager.findBoundingBoxesPresentInFrame(frameId);
    YAML::Node ymlFile;
    ymlFile["meta"] = YAML::Node();
    ymlFile["objects"] = YAML::Node();

    ymlFile["meta"]["date"] = std::string("N.A.");
    ymlFile["meta"]["source"] = std::string("N.A.");

    for (auto bbId : bbsPresent)
    {
      auto* bb = bboxManager.getBoundingBoxFromIndex(bbId);
      YAML::Node bbNode;
      bbNode["label"] = bb->getClass();

      Eigen::Isometry3d worldBoxPose = bb->getPose(frameId);
      Eigen::Isometry3d lidarBoxPose = lidarFramePoseInv * worldBoxPose;

      Eigen::Vector3d center = lidarBoxPose.translation();
      bbNode["selector"] = YAML::Node();
      bbNode["selector"]["center"].push_back(center.x());
      bbNode["selector"]["center"].push_back(center.y());
      bbNode["selector"]["center"].push_back(center.z());

      Eigen::Vector3d dimensions = bb->getDimensions();
      bbNode["selector"]["dimensions"].push_back(dimensions.x());
      bbNode["selector"]["dimensions"].push_back(dimensions.y());
      bbNode["selector"]["dimensions"].push_back(dimensions.z());
      bbNode["selector"]["type"] = "3D bounding box";


      Eigen::EulerAnglesXYZd rot(lidarBoxPose.matrix().block<3, 3>(0, 0));
      bbNode["selector"]["orientation"].push_back(rot.alpha());
      bbNode["selector"]["orientation"].push_back(rot.beta());
      bbNode["selector"]["orientation"].push_back(rot.gamma());


      if (bb->getState() == BoundingBox::State::STATIC)
      {
        bbNode["selector"]["state"] = "static";
      }
      else if (bb->getState() == BoundingBox::State::DYNAMIC)
      {
        bbNode["selector"]["state"] = "dynamic";
      }

      bbNode["id"] = bb->getInstanceId();

      ymlFile["objects"].push_back(bbNode);
    }

    std::stringstream ss;
    ss << name.generic_string() << "_" << std::setw(6) << std::setfill('0') << frameId << ".yaml";
    fs::path yamlName(ss.str());
    std::ofstream fout((directory / yamlName).generic_string());
    fout << ymlFile;
    fout.close();

    Json::Value node;
    node["name"] = yamlName.generic_string();
    node["time"] = lidarFramesManager.getFrameTimestamp(frameId);
    files.append(node);
  }

  outputSeries["files"] = files;
  outputSeries["file-series-version"] = "1.0";
  outputSeries["build_commit_hash"] = std::string(BUILD_COMMIT_HASH);    // this is filled by cmake

  std::ofstream outputFileSeries(filename);
  outputFileSeries << outputSeries;
  outputFileSeries.close();
}


bool loadBBoxDataSet(const std::string &filename)
{
  auto& lidarFramesManager = LidarFrameManager::getInstance();

  const auto& listOfBBFilesAndTimes = loadSeriesFile(filename);

  std::unordered_map<BoundingBox::Id, std::shared_ptr<BoundingBox>> bbMap;

  const int N = std::min(lidarFramesManager.getNbFrames(), listOfBBFilesAndTimes.size());
  try
  {
    for (int frameId = 0; frameId < N ; ++frameId)
    {
      Eigen::Isometry3d lidarFramePose = lidarFramesManager.getFramePose(frameId);
      YAML::Node node = YAML::LoadFile(listOfBBFilesAndTimes[frameId].first);
      YAML::Node objects = node["objects"];
      for (size_t i = 0; i < objects.size(); ++i)
      {
        auto label = objects[i]["label"].as<std::string>();
        auto instanceId = objects[i]["id"].as<unsigned int>();

        YAML::Node selector = objects[i]["selector"];
        auto center = selector["center"].as<std::vector<double>>();
        auto dimensions = selector["dimensions"].as<std::vector<double>>();

        std::vector<double> orientation = {0.0, 0.0, 0.0};
        if (selector["orientation"])   // if the orientation is not defined in the file, the orientation is identity by default
        {
          orientation = selector["orientation"].as<std::vector<double>>();
        }
        std::string stateStr = "dynamic";   // if the state is not defined in the file, the object is dynamic by default
        if (selector["state"])
        {
          stateStr = selector["state"].as<std::string>();
        }

        BoundingBox::State state = BoundingBox::State::STATIC;
        if (stateStr == "dynamic")
          state = BoundingBox::State::DYNAMIC;

        Eigen::EulerAnglesXYZd rot(orientation[0], orientation[1], orientation[2]);
        Eigen::Isometry3d pose = lidarFramePose * Eigen::Translation3d(center[0], center[1], center[2]) * rot;

        if (bbMap.find(instanceId) == bbMap.end())
        {
          // storing id
          bbMap[instanceId] = std::make_shared<BoundingBox>(0, instanceId, label, Eigen::Vector3d(dimensions[0], dimensions[1], dimensions[2]), state);
        }
        bbMap[instanceId]->addPresenceInFrame(pose, frameId);
        bbMap[instanceId]->setDimensions(Eigen::Vector3d(dimensions[0], dimensions[1], dimensions[2]));
      }
    }
  }
  catch (std::exception& e)
  {
    qCritical() << "Error while loading the BBox dataset: " << QString::fromStdString(e.what());
    return false;
  }

  if (lidarFramesManager.getNbFrames() < listOfBBFilesAndTimes.size())
  {
    qWarning() << "Some BBox files were not loaded because they do not correspond to any lidar frame";
  }

  // fill the BBox Manager
  // the function in MainWindow handles the actors
  auto& listOfBBoxes = BoundingBoxManager::getInstance().getBoundingBoxes();
  for (auto it = bbMap.begin(); it != bbMap.end(); ++it)
  {
    listOfBBoxes.push_back(it->second);
    listOfBBoxes.back()->setStoringId(listOfBBoxes.size()-1);
  }
  return true;
}

bool loadCameraDataSet(const std::string &filename)
{
  auto& lidarFramesManager = LidarFrameManager::getInstance();
  auto& cameraFramesManager = CameraFramesManager::getInstance();

  const auto& listOfImagesFilesAndTimes = loadSeriesFile(filename);

  try
  {
    for (const auto& pairFileTime : listOfImagesFilesAndTimes)
    {
      std::string filename = pairFileTime.first;
      double time = pairFileTime.second;
      cameraFramesManager.addFrame(filename, time);
    }
  }
  catch (std::exception& e)
  {
    qCritical() << "Error while loading the Camera dataset: " << QString::fromStdString(e.what());
    return false;
  }

  cameraFramesManager.finalizeAddingFrames();
  return true;
}
