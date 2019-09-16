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

#ifndef BOUNDINGBOX_H
#define BOUNDINGBOX_H

#include <vector>

#include <Eigen/Dense>

#include <vtkMatrix4x4.h>
#include <vtkSmartPointer.h>


/// This class represents a bounding box
class BoundingBox
{

public:

  using  Id = unsigned int;

  enum class State
  {
    STATIC,
    DYNAMIC
  };

  /// Constructor
  BoundingBox(Id storingId, Id instanceId, const std::string& classe, const Eigen::Vector3d &dimensions, State state);


  /// Check if the bounding box is present in frame frameId
  bool isInFrame(unsigned int frameId);


  /// Get a list of frames in which the bounding box is present
  const std::vector<unsigned int>& getFrames() const;

  /// Get a list of poses of the bounding box in the different frames
  const std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>>& getPoses() const;


  /// Add the presence of the bounding box in a frame. This also adds the corresponding pose
  void addPresenceInFrame(const Eigen::Isometry3d& pose,  unsigned int frameId);

  /// Remove the presence of the bounding box in a frame. This also removes the corresponding pose
  void removePresenceInFrame(unsigned int frameId);

  /// Return true if the bounding box appears in at least one frame
  bool hasAnyPresence() const;

  /// Return true if the bounding box is present in the interval (both inclusive)
  bool isPresentInInterval(int first, int last) const;

  /// Returns the the Id of the first frame of the interval where the BBox is present.
  /// If the BBox is not present in the interval it returns -1
  int getFirstFrameOfPresenceInInterval(int first, int last) const;


  /// Get / Set the pose of the bounding box in frame frameId
  Eigen::Isometry3d getPose(unsigned int frameId) const;
  bool setPose(unsigned int frameId, const Eigen::Isometry3d &pose);

  /// Get / Set the class
  const std::string& getClass() const;
  void setClass(const std::string& classe);

  /// Get the instance id
  Id getInstanceId() const;

  /// Get / Set the storing id
  Id getStoringId() const;
  void setStoringId(Id id);

  // Get / Set the dimensions
  Eigen::Vector3d getDimensions() const;
  void setDimensions(const Eigen::Vector3d& dims);

  /// Get / Set the state
  State getState() const;
  void setState(State state);

  /// Get the center of the bounding box in frameId
  Eigen::Vector3d getCenter(unsigned int frameId) const;


  /// Get the orientation of the bounding box in frameId
  Eigen::Quaterniond getOrientation(unsigned int frameId) const;


private:

  Id m_storingId;     // storing id inside the vector of bounding boxes
  Id m_instanceId;    // unique per object

  State m_state;

  // for now the dimensions of the bounding is constant in every frame
  Eigen::Vector3d m_dimensions;

  std::string m_class;

  // vector of poses and the corresponding frame id
  std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d>> m_poses;
  std::vector<unsigned int> m_framesIds;


  EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};



inline bool BoundingBox::isInFrame(unsigned int frameId)
{
  return std::find(m_framesIds.begin(), m_framesIds.end(), frameId) != m_framesIds.end();
}

inline bool BoundingBox::hasAnyPresence() const
{
  return m_framesIds.size() > 0;
}

inline const std::vector<unsigned int> &BoundingBox::getFrames() const
{
  return m_framesIds;
}

inline const std::vector<Eigen::Isometry3d, Eigen::aligned_allocator<Eigen::Isometry3d> > &BoundingBox::getPoses() const
{
  return m_poses;
}

inline const std::string &BoundingBox::getClass() const
{
  return m_class;
}

inline void BoundingBox::setClass(const std::string &classe)
{
  m_class = classe;
}

inline BoundingBox::Id BoundingBox::getInstanceId() const
{
  return m_instanceId;
}

inline BoundingBox::Id BoundingBox::getStoringId() const
{
  return m_storingId;
}

inline void BoundingBox::setStoringId(BoundingBox::Id id)
{
  m_storingId = id;
}

inline Eigen::Vector3d BoundingBox::getDimensions() const
{
  return m_dimensions;
}

inline void BoundingBox::setDimensions(const Eigen::Vector3d &dims)
{
  m_dimensions = dims;
}

inline BoundingBox::State BoundingBox::getState() const
{
  return m_state;
}

inline void BoundingBox::setState(BoundingBox::State state)
{
  m_state = state;
}

inline Eigen::Vector3d BoundingBox::getCenter(unsigned int frameId) const
{
  return getPose(frameId).translation();
}

inline Eigen::Quaterniond BoundingBox::getOrientation(unsigned int frameId) const
{
  return Eigen::Quaterniond(getPose(frameId).rotation());
}

#endif // BOUNDINGBOX_H

