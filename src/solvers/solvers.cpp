#include <cube/cube.hpp>
#include <solvers/solvers.hpp>

#include <iostream>
#include <memory>
#include <vector>

using namespace Cube;

namespace Kociemba {

/*static arrays for basic symmetry operations*/
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
  for (int j = 0; j < NUMBER_OF_SYMMETRIES; j++) {
    for (int i = 0; i < NUMBER_OF_SYMMETRIES; i++) {
      EdgeCornerCube tempCube = symCubes_[j];
      tempCube.multiply(symCubes_[i]);

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
    auto cornerTuple = ecCube_.getCorner(cornerID);  // Use a temporary variable
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

/*Coordinate Cube method implementations*/
// Constructor
KociembaSolver::CoordinateCube::CoordinateCube(KociembaSolver* solver) : solver_{solver} {
  coordsPhaseOne = phaseOne(getTwist(), getFlip(), getSliceSorted());
  coordsPhaseTwo = phaseTwo(getUEdges(), getDEdges(), getCorners(), getUDEdges());
}

/*Coordinate mappings between EdgeCornerCube and CoordinateCube*/
int KociembaSolver::CoordinateCube::getTwist() const {
  int twist = 0;
  // Iterate from URF to DRB (not including DRB)
  for (int i = static_cast<int>(CornerID::URF); i < static_cast<int>(CornerID::DRB); i++) {
    // Build the 3-base number by multiplying by 3 and adding corner orientation
    twist = 3 * twist + solver_->ecCube_.getCorner(i).orientation;
  }
  return twist;
}

void KociembaSolver::CoordinateCube::setTwist(int twist) {
  int twistparity = 0;
  std::array<Cube::Corner, 8> temp_corners;
  // Copy current corner state
  for (int i = 0; i < 8; ++i) {
    temp_corners[i] = solver_->ecCube_.getCorner(i);
  }

  // Process corners from DRB-1 down to URF
  for (int i = static_cast<int>(CornerID::DRB) - 1; i >= static_cast<int>(CornerID::URF); --i) {
    // Extract orientation from twist value
    temp_corners[i].orientation = twist % 3;
    twistparity += temp_corners[i].orientation;
    twist /= 3;
  }

  // Set the last corner (DRB) to ensure total twist is 0 mod 3
  temp_corners[static_cast<int>(CornerID::DRB)].orientation = ((3 - twistparity % 3) % 3);

  // Update the cube with the new corners
  for (int i = 0; i < 8; ++i) {
    solver_->ecCube_.setCorner(i, temp_corners[i]);
  }
  coordsPhaseOne.twist = twist;
}

int KociembaSolver::CoordinateCube::getFlip() const {
  int flip = 0;
  for (int i = static_cast<int>(EdgeID::UR); i < static_cast<int>(EdgeID::BR); i++) {
    flip = 2 * flip + solver_->ecCube_.getEdge(i).orientation;
  }
  return flip;
}

void KociembaSolver::CoordinateCube::setFlip(int flip) {
  int flipparity = 0;
  std::array<Cube::Edge, 12> temp_edges;
  for (int i = 0; i < 12; ++i) temp_edges[i] = solver_->ecCube_.getEdge(i);

  for (int i = static_cast<int>(EdgeID::BR) - 1; i >= static_cast<int>(EdgeID::UR); --i) {
    temp_edges[i].orientation = flip % 2;
    flipparity += temp_edges[i].orientation;
    flip /= 2;
  }
  temp_edges[static_cast<int>(EdgeID::BR)].orientation = ((2 - flipparity % 2) % 2);

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges[i]);
  }

  coordsPhaseOne.twist = flip;
}

int KociembaSolver::CoordinateCube::getSlice() const {
  int a = 0;
  int x = 0;
  for (int j = static_cast<int>(EdgeID::BR); j >= static_cast<int>(EdgeID::UR); j--) {
    EdgeID edgePos = solver_->ecCube_.getEdge(j).position;
    if (edgePos >= EdgeID::FR && edgePos <= EdgeID::BR) {
      a += c_nk(11 - j, x + 1);
      x++;
    }
  }
  return a;
}

void KociembaSolver::CoordinateCube::setSlice(int idx) {
  std::array<EdgeID, 4> slice_edge = {EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};
  std::array<EdgeID, 8> other_edge = {EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB,
                                      EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB};

  std::array<Cube::Edge, 12> temp_edges;
  for (int i = 0; i < 12; ++i) temp_edges[i] = solver_->ecCube_.getEdge(i);

  for (int e_idx = 0; e_idx < 12; ++e_idx) {
    temp_edges[e_idx].position = EdgeID::COUNT;  // Invalidate all edge positions (equivalent to -1 in Python)
  }

  int a = idx;  // Location
  int x = 4;    // set slice edges
  for (int j = 0; j < 12; ++j) {
    if (a - c_nk(11 - j, x) >= 0) {
      temp_edges[j].position = slice_edge[4 - x];
      a -= c_nk(11 - j, x);
      x--;
    }
  }

  x = 0;  // set the remaining edges UR..DB
  for (int j = 0; j < 12; ++j) {
    if (temp_edges[j].position == EdgeID::COUNT) {
      temp_edges[j].position = other_edge[x];
      x++;
    }
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, {temp_edges[i].position, solver_->ecCube_.getEdge(i).orientation});
  }
}

int KociembaSolver::CoordinateCube::getSliceSorted() const {
  int a = 0, x = 0;
  std::array<EdgeID, 4> edge4{};

  // First, check if we found all 4 slice edges
  for (int j = static_cast<int>(EdgeID::BR); j >= static_cast<int>(EdgeID::UR); j--) {
    EdgeID edgePos = solver_->ecCube_.getEdge(j).position;
    if (edgePos >= EdgeID::FR && edgePos <= EdgeID::BR) {
      a += c_nk(11 - j, x + 1);
      edge4[3 - x] = edgePos;
      x++;
    }
  }

  // Safety check - make sure we found exactly 4 slice edges
  if (x != 4) {
    // Return a default value rather than entering infinite loop
    return 999'999;
  }

  int b = 0;
  for (int j = 3; j > 0; j--) {
    int k = 0;
    while (edge4[j] != static_cast<EdgeID>(j + 8)) {
      rotate_left(edge4, 0, j);
      k++;
    }
    b = (j + 1) * b + k;
  }
  return 24 * a + b;
}

void KociembaSolver::CoordinateCube::setSliceSorted(int idx) {
  std::array<EdgeID, 4> slice_edge = {EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};
  std::array<EdgeID, 8> other_edge = {EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB,
                                      EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB};

  std::array<Cube::Edge, 12> temp_edges;
  for (int i = 0; i < 12; ++i) {
    temp_edges[i] = solver_->ecCube_.getEdge(i);
    temp_edges[i].position = EdgeID::COUNT;
  }

  int b = idx % 24;  // Permutation
  int a = idx / 24;  // Location

  int j_rot = 1;
  while (j_rot < 4) {
    int k = b % (j_rot + 1);
    b /= (j_rot + 1);
    while (k > 0) {
      rotate_right(slice_edge, 0, j_rot);
      k--;
    }
    j_rot++;
  }

  int x = 4;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (a - c_nk(11 - j_slot, x) >= 0) {
      temp_edges[j_slot].position = slice_edge[4 - x];
      a -= c_nk(11 - j_slot, x);
      x--;
    }
  }

  x = 0;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (temp_edges[j_slot].position == EdgeID::COUNT) {
      temp_edges[j_slot].position = other_edge[x];
      x++;
    }
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges[i]);
  }
  coordsPhaseOne.slice_sorted = idx;
}

int KociembaSolver::CoordinateCube::getUEdges() const {
  int a = 0, x = 0;
  std::array<EdgeID, 4> edge4{};

  std::array<EdgeID, 12> ep_mod;
  for (int i = 0; i < 12; i++) {
    ep_mod[i] = solver_->ecCube_.getEdge(i).position;
  }

  for (int j = 0; j < 4; j++) {
    rotate_right(ep_mod, 0, 11);
  }

  for (int j = static_cast<int>(EdgeID::BR); j >= static_cast<int>(EdgeID::UR); j--) {
    if (ep_mod[j] >= EdgeID::UR && ep_mod[j] <= EdgeID::UB) {
      a += c_nk(11 - j, x + 1);
      edge4[3 - x] = ep_mod[j];
      x++;
    }
  }

  if (x != 4) {
    return 0;
  }

  int b = 0;
  for (int j = 3; j > 0; j--) {
    int k = 0;
    int safety_counter = 0;
    while (edge4[j] != static_cast<EdgeID>(j)) {
      rotate_left(edge4, 0, j);
      k++;
      safety_counter++;
      if (safety_counter > 24) {
        return 0;
      }
    }
    b = (j + 1) * b + k;
  }

  return 24 * a + b;
}

void KociembaSolver::CoordinateCube::setUEdges(int idx) {
  std::array<EdgeID, 4> slice_edge = {EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB};
  std::array<EdgeID, 8> other_edge = {EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB,
                                      EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};

  std::array<Cube::Edge, 12> temp_edges;
  for (int i = 0; i < 12; ++i) {
    temp_edges[i] = solver_->ecCube_.getEdge(i);
    temp_edges[i].position = EdgeID::COUNT;
  }

  int b = idx % 24;
  int a = idx / 24;

  int j_rot = 1;
  while (j_rot < 4) {
    int k = b % (j_rot + 1);
    b /= (j_rot + 1);
    while (k > 0) {
      rotate_right(slice_edge, 0, j_rot);
      k--;
    }
    j_rot++;
  }

  int x = 4;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (a - c_nk(11 - j_slot, x) >= 0) {
      temp_edges[j_slot].position = slice_edge[4 - x];
      a -= c_nk(11 - j_slot, x);
      x--;
    }
  }

  x = 0;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (temp_edges[j_slot].position == EdgeID::COUNT) {
      temp_edges[j_slot].position = other_edge[x];
      x++;
    }
  }

  for (int j = 0; j < 4; j++) {
    rotate_left(temp_edges, 0, 11);
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges[i]);
  }

  coordsPhaseTwo.u_edges = idx;
}

int KociembaSolver::CoordinateCube::getDEdges() const {
  int a = 0, x = 0;
  std::array<EdgeID, 4> edge4{};

  std::array<EdgeID, 12> ep_mod;
  for (int i = 0; i < 12; i++) {
    ep_mod[i] = solver_->ecCube_.getEdge(i).position;
  }

  for (int j = 0; j < 4; j++) {
    rotate_right(ep_mod, 0, 11);
  }

  for (int j = static_cast<int>(EdgeID::BR); j >= static_cast<int>(EdgeID::UR); j--) {
    if (ep_mod[j] >= EdgeID::DR && ep_mod[j] <= EdgeID::DB) {
      a += c_nk(11 - j, x + 1);
      edge4[3 - x] = ep_mod[j];
      x++;
    }
  }

  if (x != 4) {
    return 0;
  }

  int b = 0;
  for (int j = 3; j > 0; j--) {
    int k = 0;
    int safety_counter = 0;
    while (edge4[j] != static_cast<EdgeID>(j + 4)) {
      rotate_left(edge4, 0, j);
      k++;
      safety_counter++;
      if (safety_counter > 24) {
        return 0;
      }
    }
    b = (j + 1) * b + k;
  }

  return 24 * a + b;
}

void KociembaSolver::CoordinateCube::setDEdges(int idx) {
  std::array<EdgeID, 4> slice_edge = {EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB};
  std::array<EdgeID, 8> other_edge = {EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR,
                                      EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB};

  std::array<Cube::Edge, 12> temp_edges;
  for (int i = 0; i < 12; ++i) {
    temp_edges[i] = solver_->ecCube_.getEdge(i);
    temp_edges[i].position = EdgeID::COUNT;
  }

  int b = idx % 24;
  int a = idx / 24;

  int j_rot = 1;
  while (j_rot < 4) {
    int k = b % (j_rot + 1);
    b /= (j_rot + 1);
    while (k > 0) {
      rotate_right(slice_edge, 0, j_rot);
      k--;
    }
    j_rot++;
  }

  int x = 4;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (a - c_nk(11 - j_slot, x) >= 0) {
      temp_edges[j_slot].position = slice_edge[4 - x];
      a -= c_nk(11 - j_slot, x);
      x--;
    }
  }

  x = 0;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (temp_edges[j_slot].position == EdgeID::COUNT) {
      temp_edges[j_slot].position = other_edge[x];
      x++;
    }
  }

  for (int j = 0; j < 4; j++) {
    rotate_left(temp_edges, 0, 11);
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges[i]);
  }

  coordsPhaseTwo.d_edges = idx;
}

int KociembaSolver::CoordinateCube::getCorners() const {
  std::array<CornerID, 8> perm;
  for (int i = 0; i < 8; ++i) {
    perm[i] = solver_->ecCube_.getCorner(i).position;
  }

  int b = 0;
  for (int j = static_cast<int>(CornerID::DRB); j > static_cast<int>(CornerID::URF); j--) {
    int k = 0;
    int safety_counter = 0;
    while (perm[j] != static_cast<CornerID>(j)) {
      rotate_left(perm, 0, j);
      k++;
      safety_counter++;
      if (safety_counter > 24) {
        return 0;
      }
    }
    b = (j + 1) * b + k;
  }
  return b;
}

void KociembaSolver::CoordinateCube::setCorners(int idx) {
  std::array<CornerID, 8> cp_target;
  for (int i = 0; i < 8; ++i) cp_target[i] = static_cast<CornerID>(i);

  for (int j = 0; j < 8; ++j) {
    int k = idx % (j + 1);
    idx /= (j + 1);
    while (k > 0) {
      rotate_right(cp_target, 0, j);
      k--;
    }
  }

  for (int i = 0; i < 8; ++i) {
    solver_->ecCube_.setCorner(i, {cp_target[i], solver_->ecCube_.getCorner(i).orientation});
  }

  coordsPhaseTwo.corners = idx;
}

int KociembaSolver::CoordinateCube::getUDEdges() const {
  if (getSliceSorted() >= 24) {
    return -1;
  }

  std::array<EdgeID, 8> perm;
  for (int i = 0; i <= static_cast<int>(EdgeID::DB); ++i) {
    perm[i] = solver_->ecCube_.getEdge(i).position;
  }

  int b = 0;
  for (int j = static_cast<int>(EdgeID::DB); j > static_cast<int>(EdgeID::UR); --j) {
    int k = 0;
    int safety_counter = 0;
    while (perm[j] != static_cast<EdgeID>(j)) {
      rotate_left(perm, 0, j);
      k++;
    }
    b = (j + 1) * b + k;
  }
  return b;
}

void KociembaSolver::CoordinateCube::setUDEdges(int idx) {
  std::array<EdgeID, 12> ep_target;

  for (int i = 0; i < 12; ++i) {
    if (i < 8) {
      ep_target[i] = static_cast<EdgeID>(i);
    } else {
      ep_target[i] = solver_->ecCube_.getEdge(i).position;
    }
  }

  for (int j = 0; j < 8; ++j) {
    int k = idx % (j + 1);
    idx /= (j + 1);
    while (k > 0) {
      rotate_right(ep_target, 0, j);
      k--;
    }
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, {ep_target[i], solver_->ecCube_.getEdge(i).orientation});
  }

  coordsPhaseTwo.ud_edges = idx;
}

/*KociembaSolver - misc*/
KociembaSolver::KociembaSolver(const Cube::EdgeCornerCube& cube) : ecCube_{cube}, coordCube{this} {}

std::string KociembaSolver::toString() const { return ecCube_.toString(); }

/*helpers for coordinate computation*/
int KociembaSolver::CoordinateCube::c_nk(int n, int k) {
  if (n < k) {
    return 0;
  }
  if (k > n / 2) {
    k = n - k;
  }

  int s = 1;
  int i = n;
  int j = 1;
  while (i != n - k) {
    s *= i;
    s /= j;
    i--;
    j++;
  }
  return s;
}

template <typename Container>
void KociembaSolver::CoordinateCube::rotate_right(Container& arr, int left, int right) {
  auto temp = arr[right];
  for (int i = right; i > left; i--) {
    arr[i] = arr[i - 1];
  }
  arr[left] = temp;
}

template <typename Container>
void KociembaSolver::CoordinateCube::rotate_left(Container& arr, int left, int right) {
  auto temp = arr[left];
  for (int i = left; i < right; i++) {
    arr[i] = arr[i + 1];
  }
  arr[right] = temp;
}

}  // namespace Kociemba