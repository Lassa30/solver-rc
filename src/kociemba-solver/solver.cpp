#include <cube/cube.hpp>
#include <kociemba-solver/solver.hpp>

#include <array>
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using namespace Cube;

namespace Kociemba {

/*symmetries*/
EdgeCornerCube KociembaSolver::inverseCube() const {
  std::array<Corner, 8> invCorners;
  std::array<Edge, 12> invEdges;

  /*nb: 
    using two for-loops for edges and corners is required
    - calculation of edge/corner orientations depend on edge/corner positions
    -> edge/corner positions must be calculated at the first place
  */

  /*inverting edges*/
  for (int edgeID = 0; edgeID < (int)EdgeID::COUNT; edgeID++) {
    int edgePosIdx = static_cast<int>(ecCube_.getEdge(edgeID).position);
    invEdges[edgePosIdx].position = static_cast<EdgeID>(edgeID);
  }
  for (int edgeID = 0; edgeID < (int)EdgeID::COUNT; edgeID++) {
    int edgeOriIdx = static_cast<int>(invEdges[edgeID].position);
    invEdges[edgeID].orientation = ecCube_.getEdge(edgeOriIdx).orientation;
  }

  /*inverting corners*/
  for (int cornerID = 0; cornerID < (int)CornerID::COUNT; cornerID++) {
    auto cornerTuple = ecCube_.getCorner(cornerID);
    CornerID cornerPos = cornerTuple.position;
    invCorners[static_cast<int>(cornerPos)].position = static_cast<CornerID>(cornerID);
  }

  int cornerOrientation = 0;
  for (int cornerID = 0; cornerID < (int)CornerID::COUNT; cornerID++) {
    cornerOrientation = ecCube_.getCorner(static_cast<int>(invCorners[cornerID].position)).orientation;
    int& newOrientation = invCorners[cornerID].orientation;
    if (cornerOrientation >= 3) {
      newOrientation = cornerOrientation;
    } else {
      newOrientation = -cornerOrientation;
      newOrientation = (newOrientation < 0) ? newOrientation + 3 : newOrientation;
    }
  }

  return EdgeCornerCube(invEdges, invCorners);
}

std::bitset<96> KociembaSolver::symmetriesCube() const {
  std::bitset<96> symmetries;
  EdgeCornerCube tempCube;
  EdgeCornerCube invCube = inverseCube();

  for (int j = 0; j < NUMBER_OF_SYMMETRIES; j++) {
    tempCube = symCubes_[j];
    tempCube.multiply(ecCube_);
    tempCube.multiply(symCubes_[inverseSymIdx_[j]]);

    if (tempCube == ecCube_) {
      symmetries[j] = true;
    }

    if (tempCube == invCube) {
      symmetries[j + NUMBER_OF_SYMMETRIES] = true;
    }
  }

  return symmetries;
}

/*symmetry checkers*/
bool KociembaSolver::hasRotationalSymmetry(const std::bitset<96>& symmetries) const {
  return symmetries[16] || symmetries[20] || symmetries[24] || symmetries[28];
}

bool KociembaSolver::hasAntiSymmetry(const std::bitset<96>& symmetries) const {
  for (int i = NUMBER_OF_SYMMETRIES; i < 2 * NUMBER_OF_SYMMETRIES; i++) {
    if (symmetries[i]) {
      return true;
    }
  }
  return false;
}

/*KociembaSolver - misc*/
KociembaSolver::KociembaSolver(const Cube::EdgeCornerCube& cube) : ecCube_(cube), coordCube(this) {}

std::string KociembaSolver::toString() const { return ecCube_.toString(); }

std::string KociembaSolver::getPrecomputedPath(const std::string& filename) {
  std::filesystem::path dir_path;
#ifdef PRECOMPUTED_FILES_INSTALL_DIR
  dir_path = std::filesystem::path(PRECOMPUTED_FILES_INSTALL_DIR);
#else
  dir_path = std::filesystem::current_path() / "precomputed";
#endif

  if (!std::filesystem::exists(dir_path)) {
    std::filesystem::create_directories(dir_path);
  }
  return (dir_path / filename).string();
}

unsigned char KociembaSolver::getFlipSliceTwistDepth3(int ix) {
  // Ensure flipSliceTwistDepth3_ is large enough before accessing
  if (static_cast<size_t>(ix / 16) >= KociembaSolver::flipSliceTwistDepth3_.size()) {
    return 0xFF;  // Or throw an exception
  }
  uint32_t y = KociembaSolver::flipSliceTwistDepth3_[ix / 16];
  y >>= (ix % 16) * 2;
  return static_cast<unsigned char>(y & 3);
}

void KociembaSolver::setFlipSliceTwistDepth3(int ix, unsigned char value) {
  // Ensure flipSliceTwistDepth3_ is large enough before accessing
  size_t base_idx = static_cast<size_t>(ix >> 4);  // ix / 16
  if (base_idx >= KociembaSolver::flipSliceTwistDepth3_.size()) {
    return;
  }
  unsigned int shift = (ix % 16) * 2;
  uint32_t mask = ~(static_cast<uint32_t>(3) << shift);
  KociembaSolver::flipSliceTwistDepth3_[base_idx] &= mask;
  KociembaSolver::flipSliceTwistDepth3_[base_idx] |= (static_cast<uint32_t>(value & 3) << shift);
}

unsigned char KociembaSolver::getCornersUDEdgesDepth3(int ix) {
  if (static_cast<size_t>(ix / 16) >= cornersUDEdgesDepth3_.size()) {
    return 0xFF;  // Indicates uninitialized or error
  }
  uint32_t y = cornersUDEdgesDepth3_[ix / 16];
  y >>= (ix % 16) * 2;
  return static_cast<unsigned char>(y & 3);
}

void KociembaSolver::setCornersUDEdgesDepth3(int ix, unsigned char value) {
  size_t base_idx = static_cast<size_t>(ix >> 4);  // ix / 16
  if (base_idx >= cornersUDEdgesDepth3_.size()) {
    return;  // Avoid writing out of bounds
  }
  unsigned int shift = (ix % 16) * 2;
  uint32_t mask = ~(static_cast<uint32_t>(3) << shift);
  cornersUDEdgesDepth3_[base_idx] &= mask;
  cornersUDEdgesDepth3_[base_idx] |= (static_cast<uint32_t>(value & 3) << shift);
}

// TODO: write the solver itself, it should be quite easy now!
/* **************************************************************************
begin: solver part responsible for finding the solution
****************************************************************************/

/* **************************************************************************
end: solver part responsible for finding the solution
****************************************************************************/

}  // namespace Kociemba