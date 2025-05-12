#include <cube/cube.hpp>
#include <solvers/solvers.hpp>

#include <iostream>

using namespace Cube;

/*static arrays for basic symmetry operations
No comments for symmetries would be provided
This is magic for me. I just used the reference:
`symmetries.py` in Kociemba's repo
*/

static const std::array<CornerID, 8> cpROT_URF3 = {CornerID::URF, CornerID::DFR, CornerID::DLF, CornerID::UFL,
                                                   CornerID::UBR, CornerID::DRB, CornerID::DBL, CornerID::ULB};
static const std::array<int, 8> coROT_URF3 = {1, 2, 1, 2, 2, 1, 2, 1};
static const std::array<EdgeID, 12> epROT_URF3 = {EdgeID::UF, EdgeID::FR, EdgeID::DF, EdgeID::FL,
                                                  EdgeID::UB, EdgeID::BR, EdgeID::DB, EdgeID::BL,
                                                  EdgeID::UR, EdgeID::DR, EdgeID::DL, EdgeID::UL};
static const std::array<int, 12> eoROT_URF3 = {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1};

static const std::array<CornerID, 8> cpROT_F2 = {CornerID::DLF, CornerID::DFR, CornerID::DRB, CornerID::DBL,
                                                 CornerID::UFL, CornerID::URF, CornerID::UBR, CornerID::ULB};
static const std::array<int, 8> coROT_F2 = {0, 0, 0, 0, 0, 0, 0, 0};
static const std::array<EdgeID, 12> epROT_F2 = {EdgeID::DL, EdgeID::DF, EdgeID::DR, EdgeID::DB, EdgeID::UL, EdgeID::UF,
                                                EdgeID::UR, EdgeID::UB, EdgeID::FL, EdgeID::FR, EdgeID::BR, EdgeID::BL};
static const std::array<int, 12> eoROT_F2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const std::array<CornerID, 8> cpROT_U4 = {CornerID::UBR, CornerID::URF, CornerID::UFL, CornerID::ULB,
                                                 CornerID::DRB, CornerID::DFR, CornerID::DLF, CornerID::DBL};
static const std::array<int, 8> coROT_U4 = {0, 0, 0, 0, 0, 0, 0, 0};
static const std::array<EdgeID, 12> epROT_U4 = {EdgeID::UB, EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::DB, EdgeID::DR,
                                                EdgeID::DF, EdgeID::DL, EdgeID::BR, EdgeID::FR, EdgeID::FL, EdgeID::BL};
static const std::array<int, 12> eoROT_U4 = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};

static const std::array<CornerID, 8> cpMIRR_LR2 = {CornerID::UFL, CornerID::URF, CornerID::UBR, CornerID::ULB,
                                                   CornerID::DLF, CornerID::DFR, CornerID::DRB, CornerID::DBL};
static const std::array<int, 8> coMIRR_LR2 = {3, 3, 3, 3, 3, 3, 3, 3};
static const std::array<EdgeID, 12> epMIRR_LR2 = {EdgeID::UL, EdgeID::UF, EdgeID::UR, EdgeID::UB,
                                                  EdgeID::DL, EdgeID::DF, EdgeID::DR, EdgeID::DB,
                                                  EdgeID::FL, EdgeID::FR, EdgeID::BR, EdgeID::BL};
static const std::array<int, 12> eoMIRR_LR2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/*static member definitions*/
std::array<EdgeCornerCube, static_cast<int>(BasicSymmetry::COUNT)> KociembaSolver::basicSymCubes_;
std::array<EdgeCornerCube, NUMBER_OF_SYMMETRIES> KociembaSolver::symCubes_;
std::array<int, NUMBER_OF_SYMMETRIES> KociembaSolver::inverseSymIdx_;

bool KociembaSolver::isBasicSymCubesGenerated_ = [] {
  KociembaSolver::generateBasicSymCubes();
  return true;
}();

bool KociembaSolver::isSymCubesGenerated_ = [] {
  KociembaSolver::generateSymCubes();
  return true;
}();

bool KociembaSolver::isInverseSymIdxGenerated_ = [] {
  KociembaSolver::generateInverseSymIdx();
  return true;
}();

void KociembaSolver::generateBasicSymCubes() {
  /*initialize basic symmetry cubes*/
  basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_URF3)] =
      EdgeCornerCube(epROT_URF3, eoROT_URF3, cpROT_URF3, coROT_URF3);

  basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_F2)] = EdgeCornerCube(epROT_F2, eoROT_F2, cpROT_F2, coROT_F2);

  basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_U4)] = EdgeCornerCube(epROT_U4, eoROT_U4, cpROT_U4, coROT_U4);

  basicSymCubes_[static_cast<int>(BasicSymmetry::MIRR_LR2)] =
      EdgeCornerCube(epMIRR_LR2, eoMIRR_LR2, cpMIRR_LR2, coMIRR_LR2);
}

void KociembaSolver::generateSymCubes() {
  EdgeCornerCube cc;
  int idx = 0;

  for (int urf3 = 0; urf3 < 3; urf3++) {
    for (int f2 = 0; f2 < 2; f2++) {
      for (int u4 = 0; u4 < 4; u4++) {
        for (int lr2 = 0; lr2 < 2; lr2++) {
          symCubes_[idx] = cc;
          idx++;
          cc.multiply(basicSymCubes_[static_cast<int>(BasicSymmetry::MIRR_LR2)]);
        }
        cc.multiply(basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_U4)]);
      }
      cc.multiply(basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_F2)]);
    }
    cc.multiply(basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_URF3)]);
  }
}

void KociembaSolver::generateInverseSymIdx() {
  /*find the inverse symmetry for each of the 48 symmetries*/
  for (int j = 0; j < NUMBER_OF_SYMMETRIES; j++) {
    for (int i = 0; i < NUMBER_OF_SYMMETRIES; i++) {
      /*Create a temporary cube to test if this is the inverse*/
      EdgeCornerCube tempCube = symCubes_[j];
      tempCube.multiply(symCubes_[i]);

      /*Check if this combination gives the identity cube
        In Python they check specific corner positions*/
      if (tempCube == EdgeCornerCube()) {
        inverseSymIdx_[j] = i;
        break;
      }
    }
  }
}

/*symmetries*/
EdgeCornerCube KociembaSolver::inverseCube() const {
  std::array<Corner, 8> invCorners;
  std::array<Edge, 12> invEdges;

  /*nb: 
    using two for-loops for edges and corners is required
    - calculation of edge/corner orientations depend on edge/corner positions
    -> edge/corner positions must be calculated at the first place
  */

  // Inverting edges
  for (int edgeID = 0; edgeID < (int)EdgeID::COUNT; edgeID++) {
    int edgePosIdx = static_cast<int>(ecCube_.getEdge(edgeID).position);
    invEdges[edgePosIdx].position = static_cast<EdgeID>(edgeID);
  }
  for (int edgeID = 0; edgeID < (int)EdgeID::COUNT; edgeID++) {
    int edgeOriIdx = static_cast<int>(invEdges[edgeID].position);
    invEdges[edgeID].orientation = ecCube_.getEdge(edgeOriIdx).orientation;
  }

  // Inverting corners
  for (int cornerID = 0; cornerID < (int)CornerID::COUNT; cornerID++) {
    auto [cornerPos, cornerOri] = ecCube_.getCorner(cornerID);
    invCorners[(int)cornerPos].position = static_cast<CornerID>(cornerID);
  }

  int cornerOrientation = 0;
  for (int cornerID = 0; cornerID < (int)CornerID::COUNT; cornerID++) {
    cornerOrientation = ecCube_.getCorner((int)invCorners[cornerID].position).orientation;
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

/*other stuff*/
KociembaSolver::KociembaSolver(const Cube::EdgeCornerCube& cube) : ecCube_{cube} {}

const EdgeCornerCube& KociembaSolver::getCube() const { return ecCube_; }