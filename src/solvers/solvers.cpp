#include <cube/cube.hpp>
#include <solvers/solvers.hpp>

#include <filesystem>
#include <fstream>
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

/*static member definitions for conjugated moves*/
std::array<int, NUMBER_OF_MOVES * NUMBER_OF_SYMMETRIES> KociembaSolver::conjMoves_;
bool KociembaSolver::isConjMovesGenerated_ = [] {
  KociembaSolver::generateConjMove();
  return true;
}();

/*static member definitions for twist conjugation table*/
std::vector<unsigned short> KociembaSolver::twistConjTable_;
bool KociembaSolver::isTwistConjTableGenerated_ = [] {
  KociembaSolver::generateTwistConjTable();
  return true;
}();

/*static member definitions for flip-slice symmetry tables*/
std::vector<unsigned short> KociembaSolver::flipSliceClassIdx_;
std::vector<unsigned char> KociembaSolver::flipSliceSym_;
std::vector<uint32_t> KociembaSolver::flipSliceRep_;
bool KociembaSolver::isFlipSliceTablesGenerated_ = [] {
  KociembaSolver::generateFlipSliceTables();
  return true;
}();

/*static member definitions for corner permutation symmetry tables*/
std::vector<unsigned short> KociembaSolver::cornerClassIdx_;
std::vector<unsigned char> KociembaSolver::cornerSym_;
std::vector<unsigned short> KociembaSolver::cornerRep_;
bool KociembaSolver::isCornerTablesGenerated_ = [] {
  KociembaSolver::generateCornerTables();
  return true;
}();

/*static member definitions for move tables*/
std::vector<unsigned short> KociembaSolver::twistMoveTable_;
bool KociembaSolver::isTwistMoveTableGenerated_ = [] {
  KociembaSolver::generateTwistMoveTable();
  return true;
}();

std::vector<unsigned short> KociembaSolver::flipMoveTable_;
bool KociembaSolver::isFlipMoveTableGenerated_ = [] {
  KociembaSolver::generateFlipMoveTable();
  return true;
}();

std::vector<unsigned short> KociembaSolver::sliceSortedMoveTable_;
bool KociembaSolver::isSliceSortedMoveTableGenerated_ = [] {
  KociembaSolver::generateSliceSortedMoveTable();
  return true;
}();

std::vector<unsigned short> KociembaSolver::uEdgesMoveTable_;
bool KociembaSolver::isUEdgesMoveTableGenerated_ = [] {
  KociembaSolver::generateUEdgesMoveTable();
  return true;
}();

std::vector<unsigned short> KociembaSolver::dEdgesMoveTable_;
bool KociembaSolver::isDEdgesMoveTableGenerated_ = [] {
  KociembaSolver::generateDEdgesMoveTable();
  return true;
}();

std::vector<unsigned short> KociembaSolver::udEdgesMoveTable_;
bool KociembaSolver::isUDEdgesMoveTableGenerated_ = [] {
  KociembaSolver::generateUDEdgesMoveTable();
  return true;
}();

std::vector<unsigned short> KociembaSolver::cornersMoveTable_;
bool KociembaSolver::isCornersMoveTableGenerated_ = [] {
  KociembaSolver::generateCornersMoveTable();
  return true;
}();

std::vector<uint32_t> KociembaSolver::cornersUDEdgesDepth3_;
bool KociembaSolver::isCornersUDEdgesDepth3TableGenerated_ = [] {
  KociembaSolver::generateCornersUDEdgesDepth3Table();
  return true;
}();

std::vector<unsigned short> KociembaSolver::udEdgesConjTable_;
bool KociembaSolver::isUDEdgesConjTableGenerated_ = [](){
  KociembaSolver::generateUDEdgesConjTable();
  return true;
};


/*static member definitions for pruning tables*/
std::vector<uint32_t> KociembaSolver::flipSliceTwistDepth3_;
std::vector<unsigned char>
    KociembaSolver::cornSliceDepth_;  // Actual initialization will occur during pruning table generation
std::vector<unsigned char>
    KociembaSolver::edgeSliceDepth_;  // Actual initialization will occur during pruning table generation

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

void KociembaSolver::generateConjMove() {
  const std::string filename = "conj_move";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      file.read(reinterpret_cast<char*>(conjMoves_.data()), conjMoves_.size() * sizeof(int));
      file.close();
    }
    return;
  }

  // File doesn't exist or couldn't be read, create the table
  for (int i = 0; i < NUMBER_OF_MOVES * NUMBER_OF_SYMMETRIES; i++) {
    conjMoves_[i] = 0;
  }

  // For each symmetry
  for (int s = 0; s < NUMBER_OF_SYMMETRIES; s++) {
    // For each possible move
    for (int m = 0; m < NUMBER_OF_MOVES; m++) {
      // Create a cube with the symmetry transformation
      EdgeCornerCube ss = symCubes_[s];

      // Apply move m to it (s*m)
      ss.multiply(EdgeCornerCube::getMoveCube(static_cast<SideMovement>(m)));

      // Apply inverse symmetry (s*m*s^-1)
      ss.multiply(symCubes_[inverseSymIdx_[s]]);

      // Find which move this is equivalent to
      for (int m2 = 0; m2 < NUMBER_OF_MOVES; m2++) {
        if (ss == EdgeCornerCube::getMoveCube(static_cast<SideMovement>(m2))) {
          conjMoves_[NUMBER_OF_MOVES * s + m] = m2;
          break;
        }
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(conjMoves_.data()), conjMoves_.size() * sizeof(int));
    file.close();
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
  coordsPhaseOne.twist =
      twist;  // This line was missing in the previous snippet, should be solver_->coordCube.coordsPhaseOne.twist
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

  coordsPhaseOne.flip = flip;  // Corrected from coordsPhaseOne.twist = flip;
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
  int a = 0;
  int x = 0;
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
      safety_counter++;
      if (safety_counter > 24) {
        return 0;
      }
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

std::string KociembaSolver::getPrecomputedPath(const std::string& filename) {
  std::filesystem::path dir_path;
#ifdef PRECOMPUTED_FILES_INSTALL_DIR
  dir_path = std::filesystem::path(PRECOMPUTED_FILES_INSTALL_DIR);
#else
  // Fallback to current working directory if install path is not defined
  dir_path = std::filesystem::current_path() / "precomputed";
#endif

  if (!std::filesystem::exists(dir_path)) {
    // Attempt to create the directory. This might fail if permissions are insufficient,
    // especially for system-wide install paths if not running with adequate privileges
    // during the first run that attempts to write.
    // Ideally, the CMake install process should create this directory.
    std::filesystem::create_directories(dir_path);
  }
  return (dir_path / filename).string();
}

void KociembaSolver::generateTwistConjTable() {
  const std::string filename = "conj_twist";
  const std::string filepath = getPrecomputedPath(filename);

  // Check if file already exists
  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    // File exists, load the table
    file_check.close();

    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      twistConjTable_.resize(TWIST_MAX * N_SYM_D4h);
      file.read(reinterpret_cast<char*>(twistConjTable_.data()), twistConjTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  // File doesn't exist, create the table
  twistConjTable_.resize(TWIST_MAX * N_SYM_D4h, 0);

  EdgeCornerCube base_cube_for_twist_setting;  // A base cube to set twist on
  KociembaSolver solver_for_twist_setting(base_cube_for_twist_setting);

  for (int t = 0; t < TWIST_MAX; t++) {
    // Create a cube with the given twist
    // We re-initialize solver_for_twist_setting.ecCube_ to a solved state before setting twist
    solver_for_twist_setting.ecCube_ = EdgeCornerCube();
    solver_for_twist_setting.coordCube.setTwist(t);  // This modifies solver_for_twist_setting.ecCube_
    EdgeCornerCube cc_with_twist = solver_for_twist_setting.getCube();

    // Apply the symmetry transformations
    for (int s = 0; s < N_SYM_D4h; s++) {
      // Copy the symmetry cube
      EdgeCornerCube ss = symCubes_[s];

      // Apply corner multiplication (s*t)
      ss.multiplyCorners(cc_with_twist);

      // Apply inverse symmetry (s*t*s^-1)
      ss.multiplyCorners(symCubes_[inverseSymIdx_[s]]);

      // Get the resulting twist
      KociembaSolver temp_solver(ss);  // Create a new solver instance with the transformed cube
      int resulting_twist = temp_solver.coordCube.getTwist();

      twistConjTable_[N_SYM_D4h * t + s] = static_cast<unsigned short>(resulting_twist);
    }
  }

  // Save the table to file
  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(twistConjTable_.data()), twistConjTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateFlipSliceTables() {
  const std::string fname1 = "fs_classidx";
  const std::string fname2 = "fs_sym";
  const std::string fname3 = "fs_rep";

  const std::string filepath1 = getPrecomputedPath(fname1);
  const std::string filepath2 = getPrecomputedPath(fname2);
  const std::string filepath3 = getPrecomputedPath(fname3);

  std::ifstream file_check1(filepath1, std::ios::binary);
  std::ifstream file_check2(filepath2, std::ios::binary);
  std::ifstream file_check3(filepath3, std::ios::binary);

  if (file_check1.good() && file_check2.good() && file_check3.good()) {
    file_check1.close();
    file_check2.close();
    file_check3.close();

    // std::cout << "Loading flipslice sym-tables..." << std::endl;

    std::ifstream file1(filepath1, std::ios::binary);
    if (file1.is_open()) {
      flipSliceClassIdx_.resize(FLIP_MAX * SLICE_MAX);
      file1.read(reinterpret_cast<char*>(flipSliceClassIdx_.data()),
                 flipSliceClassIdx_.size() * sizeof(unsigned short));
      file1.close();
    } else { /* std::cerr << "Error opening " << filepath1 << std::endl; */
      return;
    }

    std::ifstream file2(filepath2, std::ios::binary);
    if (file2.is_open()) {
      flipSliceSym_.resize(FLIP_MAX * SLICE_MAX);
      file2.read(reinterpret_cast<char*>(flipSliceSym_.data()), flipSliceSym_.size() * sizeof(unsigned char));
      file2.close();
    } else { /* std::cerr << "Error opening " << filepath2 << std::endl; */
      return;
    }

    std::ifstream file3(filepath3, std::ios::binary);
    if (file3.is_open()) {
      flipSliceRep_.resize(FLIPSLICE_CLASS_MAX);
      file3.read(reinterpret_cast<char*>(flipSliceRep_.data()), flipSliceRep_.size() * sizeof(uint32_t));
      file3.close();
    } else { /* std::cerr << "Error opening " << filepath3 << std::endl; */
      return;
    }

    return;
  }
  // Close any streams that might have been opened if not all files were present
  if (file_check1.is_open()) file_check1.close();
  if (file_check2.is_open()) file_check2.close();
  if (file_check3.is_open()) file_check3.close();

  // std::cout << "Creating flipslice sym-tables..." << std::endl;
  flipSliceClassIdx_.assign(FLIP_MAX * SLICE_MAX, INVALID_SHORT);
  flipSliceSym_.assign(FLIP_MAX * SLICE_MAX, 0);
  flipSliceRep_.assign(FLIPSLICE_CLASS_MAX, 0);  // Will be populated, ensure size is adequate

  unsigned short classidx = 0;
  EdgeCornerCube base_cube_for_ops;  // Used to set flip/slice
  KociembaSolver solver_for_ops(base_cube_for_ops);

  for (int slc = 0; slc < SLICE_MAX; ++slc) {
    solver_for_ops.ecCube_ = EdgeCornerCube();  // Reset before setting slice
    solver_for_ops.coordCube.setSlice(slc);
    // Need to preserve the slice state while iterating flips
    EdgeCornerCube cube_with_slice_set = solver_for_ops.getCube();

    for (int flip = 0; flip < FLIP_MAX; ++flip) {
      // Apply flip to the cube that already has slice set
      solver_for_ops.ecCube_ = cube_with_slice_set;
      solver_for_ops.coordCube.setFlip(flip);

      uint32_t idx = static_cast<uint32_t>(FLIP_MAX * slc + flip);

      // if ((idx + 1) % 4000 == 0) {
      //   std::cout << "." << std::flush;
      // }
      // if ((idx + 1) % 320000 == 0) {
      //   std::cout << std::endl;
      // }

      if (flipSliceClassIdx_[idx] == INVALID_SHORT) {
        if (classidx >= FLIPSLICE_CLASS_MAX) {
          // This should not happen if FLIPSLICE_CLASS_MAX is correct
          // std::cerr << "Error: classidx exceeds FLIPSLICE_CLASS_MAX" << std::endl;
          return;
        }
        flipSliceClassIdx_[idx] = classidx;
        flipSliceSym_[idx] = 0;
        flipSliceRep_[classidx] = idx;
      } else {
        continue;
      }

      EdgeCornerCube current_ec_state = solver_for_ops.getCube();

      for (int s = 0; s < N_SYM_D4h; ++s) {
        EdgeCornerCube ss = symCubes_[inverseSymIdx_[s]];  // s^-1
        ss.multiplyEdges(current_ec_state);                // s^-1 * cc
        ss.multiplyEdges(symCubes_[s]);                    // s^-1 * cc * s

        KociembaSolver temp_solver_for_ss(ss);
        int new_slice = temp_solver_for_ss.coordCube.getSlice();
        int new_flip = temp_solver_for_ss.coordCube.getFlip();
        uint32_t idx_new = static_cast<uint32_t>(FLIP_MAX * new_slice + new_flip);

        if (flipSliceClassIdx_[idx_new] == INVALID_SHORT) {
          flipSliceClassIdx_[idx_new] = classidx;
          flipSliceSym_[idx_new] = static_cast<unsigned char>(s);
        }
      }
      classidx++;
    }
  }
  // std::cout << std::endl;
  // std::cout << "Generated " << classidx << " classes." << std::endl;

  std::ofstream file1_out(filepath1, std::ios::binary);
  if (file1_out.is_open()) {
    file1_out.write(reinterpret_cast<const char*>(flipSliceClassIdx_.data()),
                    flipSliceClassIdx_.size() * sizeof(unsigned short));
    file1_out.close();
  }  // else { std::cerr << "Error writing " << filepath1 << std::endl; }

  std::ofstream file2_out(filepath2, std::ios::binary);
  if (file2_out.is_open()) {
    file2_out.write(reinterpret_cast<const char*>(flipSliceSym_.data()), flipSliceSym_.size() * sizeof(unsigned char));
    file2_out.close();
  }  // else { std::cerr << "Error writing " << filepath2 << std::endl; }

  // Resize flipSliceRep_ to actual number of classes before saving if it was over-allocated
  // or if classidx is the definitive count. Given Python pre-allocates, using FLIPSLICE_CLASS_MAX is likely intended.
  // If classidx is less than FLIPSLICE_CLASS_MAX, the rest of flipSliceRep_ will be 0.
  // flipSliceRep_.resize(classidx); // Optional: if FLIPSLICE_CLASS_MAX is an upper bound.
  std::ofstream file3_out(filepath3, std::ios::binary);
  if (file3_out.is_open()) {
    file3_out.write(reinterpret_cast<const char*>(flipSliceRep_.data()), flipSliceRep_.size() * sizeof(uint32_t));
    file3_out.close();
  }  // else { std::cerr << "Error writing " << filepath3 << std::endl; }
}

void KociembaSolver::generateCornerTables() {
  const std::string fname1 = "co_classidx";
  const std::string fname2 = "co_sym";
  const std::string fname3 = "co_rep";

  const std::string filepath1 = getPrecomputedPath(fname1);
  const std::string filepath2 = getPrecomputedPath(fname2);
  const std::string filepath3 = getPrecomputedPath(fname3);

  std::ifstream file_check1(filepath1, std::ios::binary);
  std::ifstream file_check2(filepath2, std::ios::binary);
  std::ifstream file_check3(filepath3, std::ios::binary);

  if (file_check1.good() && file_check2.good() && file_check3.good()) {
    file_check1.close();
    file_check2.close();
    file_check3.close();

    // std::cout << "Loading corner sym-tables..." << std::endl;

    std::ifstream file1(filepath1, std::ios::binary);
    if (file1.is_open()) {
      cornerClassIdx_.resize(CORNERS_MAX);
      file1.read(reinterpret_cast<char*>(cornerClassIdx_.data()), cornerClassIdx_.size() * sizeof(unsigned short));
      file1.close();
    } else { /* std::cerr << "Error opening " << filepath1 << std::endl; */
      return;
    }

    std::ifstream file2(filepath2, std::ios::binary);
    if (file2.is_open()) {
      cornerSym_.resize(CORNERS_MAX);
      file2.read(reinterpret_cast<char*>(cornerSym_.data()), cornerSym_.size() * sizeof(unsigned char));
      file2.close();
    } else { /* std::cerr << "Error opening " << filepath2 << std::endl; */
      return;
    }

    std::ifstream file3(filepath3, std::ios::binary);
    if (file3.is_open()) {
      cornerRep_.resize(CORNERS_CLASS_MAX);
      file3.read(reinterpret_cast<char*>(cornerRep_.data()), cornerRep_.size() * sizeof(unsigned short));
      file3.close();
    } else { /* std::cerr << "Error opening " << filepath3 << std::endl; */
      return;
    }

    return;
  }
  if (file_check1.is_open()) file_check1.close();
  if (file_check2.is_open()) file_check2.close();
  if (file_check3.is_open()) file_check3.close();

  // std::cout << "Creating corner sym-tables..." << std::endl;
  cornerClassIdx_.assign(CORNERS_MAX, INVALID_SHORT);
  cornerSym_.assign(CORNERS_MAX, 0);
  cornerRep_.assign(CORNERS_CLASS_MAX, 0);

  unsigned short classidx = 0;
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);

  for (int cp = 0; cp < CORNERS_MAX; ++cp) {
    solver_for_ops.ecCube_ = EdgeCornerCube();  // Reset to solved state before setting corners
    solver_for_ops.coordCube.setCorners(cp);

    // if ((cp + 1) % 8000 == 0) {
    //   std::cout << "." << std::flush;
    // }

    if (cornerClassIdx_[cp] == INVALID_SHORT) {
      if (classidx >= CORNERS_CLASS_MAX) {
        // std::cerr << "Error: classidx exceeds CORNERS_CLASS_MAX" << std::endl;
        return;
      }
      cornerClassIdx_[cp] = classidx;
      cornerSym_[cp] = 0;
      cornerRep_[classidx] = static_cast<unsigned short>(cp);
    } else {
      continue;
    }

    EdgeCornerCube current_ec_state = solver_for_ops.getCube();

    for (int s = 0; s < N_SYM_D4h; ++s) {
      EdgeCornerCube ss = symCubes_[inverseSymIdx_[s]];  // s^-1
      ss.multiplyCorners(current_ec_state);              // s^-1 * cc
      ss.multiplyCorners(symCubes_[s]);                  // s^-1 * cc * s

      KociembaSolver temp_solver_for_ss(ss);
      int cp_new = temp_solver_for_ss.coordCube.getCorners();

      if (cornerClassIdx_[cp_new] == INVALID_SHORT) {
        cornerClassIdx_[cp_new] = classidx;
        cornerSym_[cp_new] = static_cast<unsigned char>(s);
      }
    }
    classidx++;
  }
  // std::cout << std::endl;
  // std::cout << "Generated " << classidx << " corner classes." << std::endl;

  std::ofstream file1_out(filepath1, std::ios::binary);
  if (file1_out.is_open()) {
    file1_out.write(reinterpret_cast<const char*>(cornerClassIdx_.data()),
                    cornerClassIdx_.size() * sizeof(unsigned short));
    file1_out.close();
  }  // else { std::cerr << "Error writing " << filepath1 << std::endl; }

  std::ofstream file2_out(filepath2, std::ios::binary);
  if (file2_out.is_open()) {
    file2_out.write(reinterpret_cast<const char*>(cornerSym_.data()), cornerSym_.size() * sizeof(unsigned char));
    file2_out.close();
  }  // else { std::cerr << "Error writing " << filepath2 << std::endl; }

  std::ofstream file3_out(filepath3, std::ios::binary);
  if (file3_out.is_open()) {
    file3_out.write(reinterpret_cast<const char*>(cornerRep_.data()), cornerRep_.size() * sizeof(unsigned short));
    file3_out.close();
  }  // else { std::cerr << "Error writing " << filepath3 << std::endl; }
}

void KociembaSolver::generateTwistMoveTable() {
  const std::string filename = "move_twist";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      twistMoveTable_.resize(TWIST_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(twistMoveTable_.data()), twistMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  twistMoveTable_.assign(TWIST_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < TWIST_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setTwist(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyCorners(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        twistMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getTwist());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(twistMoveTable_.data()), twistMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateFlipMoveTable() {
  const std::string filename = "move_flip";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      flipMoveTable_.resize(FLIP_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(flipMoveTable_.data()), flipMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  flipMoveTable_.assign(FLIP_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < FLIP_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setFlip(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        flipMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getFlip());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(flipMoveTable_.data()), flipMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateSliceSortedMoveTable() {
  const std::string filename = "move_slice_sorted";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      sliceSortedMoveTable_.resize(SLICE_SORTED_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(sliceSortedMoveTable_.data()),
                sliceSortedMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  sliceSortedMoveTable_.assign(SLICE_SORTED_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < SLICE_SORTED_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setSliceSorted(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        sliceSortedMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getSliceSorted());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(sliceSortedMoveTable_.data()),
               sliceSortedMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateUEdgesMoveTable() {
  const std::string filename = "move_u_edges";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      uEdgesMoveTable_.resize(SLICE_SORTED_MAX * NUMBER_OF_MOVES);  // N_SLICE_SORTED in Python
      file.read(reinterpret_cast<char*>(uEdgesMoveTable_.data()), uEdgesMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  uEdgesMoveTable_.assign(SLICE_SORTED_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < SLICE_SORTED_MAX; ++i) {  // Iterating up to N_SLICE_SORTED
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setUEdges(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        uEdgesMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getUEdges());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(uEdgesMoveTable_.data()),
               uEdgesMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateDEdgesMoveTable() {
  const std::string filename = "move_d_edges";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      dEdgesMoveTable_.resize(SLICE_SORTED_MAX * NUMBER_OF_MOVES);  // N_SLICE_SORTED in Python
      file.read(reinterpret_cast<char*>(dEdgesMoveTable_.data()), dEdgesMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  dEdgesMoveTable_.assign(SLICE_SORTED_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < SLICE_SORTED_MAX; ++i) {  // Iterating up to N_SLICE_SORTED
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setDEdges(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        dEdgesMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getDEdges());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(dEdgesMoveTable_.data()),
               dEdgesMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateUDEdgesMoveTable() {
  const std::string filename = "move_ud_edges";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      udEdgesMoveTable_.resize(UD_EDGES_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(udEdgesMoveTable_.data()), udEdgesMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  udEdgesMoveTable_.assign(UD_EDGES_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  // face_idx: 0:U, 1:R, 2:F, 3:D, 4:L, 5:B
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < UD_EDGES_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setUDEdges(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {  // power_idx 0:1x, 1:2x, 2:3x
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);

        bool skip_store = false;
        // Python: j in [R, F, L, B] and k != 1 (k=0 is 1x, k=1 is 2x, k=2 is 3x)
        // So, for R,F,L,B (face_idx 1,2,4,5), if power_idx is 0 (1x) or 2 (3x), skip storing.
        // This means only store for power_idx == 1 (2x move).
        if ((face_idx == 1 || face_idx == 2 || face_idx == 4 || face_idx == 5) && (power_idx != 1)) {
          skip_store = true;
        }

        if (!skip_store) {
          KociembaSolver temp_solver(working_cube);
          udEdgesMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
              static_cast<unsigned short>(temp_solver.coordCube.getUDEdges());
        }
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(udEdgesMoveTable_.data()),
               udEdgesMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateCornersMoveTable() {
  const std::string filename = "move_corners";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      cornersMoveTable_.resize(CORNERS_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(cornersMoveTable_.data()), cornersMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  cornersMoveTable_.assign(CORNERS_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < CORNERS_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setCorners(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyCorners(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        cornersMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getCorners());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(cornersMoveTable_.data()),
               cornersMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

unsigned char KociembaSolver::getFlipSliceTwistDepth3(int ix) {
  // Ensure flipSliceTwistDepth3_ is large enough before accessing
  if (static_cast<size_t>(ix / 16) >= flipSliceTwistDepth3_.size()) {
    // Handle error or ensure table is initialized
    // For now, returning a value indicating error or uninitialized might be appropriate
    // Or, this function should only be called after tables are properly sized.
    return 0xFF;  // Or throw an exception
  }
  uint32_t y = flipSliceTwistDepth3_[ix / 16];
  y >>= (ix % 16) * 2;
  return static_cast<unsigned char>(y & 3);
}

void KociembaSolver::setFlipSliceTwistDepth3(int ix, unsigned char value) {
  // Ensure flipSliceTwistDepth3_ is large enough before accessing
  size_t base_idx = static_cast<size_t>(ix >> 4);  // ix / 16
  if (base_idx >= flipSliceTwistDepth3_.size()) {
    // Handle error: table might need to be resized or this is an invalid index.
    // This function assumes the table is already appropriately sized.
    return;
  }
  unsigned int shift = (ix % 16) * 2;
  uint32_t mask = ~(static_cast<uint32_t>(3) << shift);
  flipSliceTwistDepth3_[base_idx] &= mask;
  flipSliceTwistDepth3_[base_idx] |= (static_cast<uint32_t>(value & 3) << shift);
}

unsigned char KociembaSolver::getCornersUDEdgesDepth3(int ix) {
  if (static_cast<size_t>(ix / 16) >= cornersUDEdgesDepth3_.size()) {
    return 0xFF;
  }
  uint32_t y = cornersUDEdgesDepth3_[ix / 16];
  y >>= (ix % 16) * 2;
  return static_cast<unsigned char>(y & 3);
}

void KociembaSolver::setCornersUDEdgesDepth3(int ix, unsigned char value) {
  size_t base_idx = static_cast<size_t>(ix >> 4);  // ix / 16
  if (base_idx >= cornersUDEdgesDepth3_.size()) {
    return;
  }
  unsigned int shift = (ix % 16) * 2;
  uint32_t mask = ~(static_cast<uint32_t>(3) << shift);
  cornersUDEdgesDepth3_[base_idx] &= mask;
  cornersUDEdgesDepth3_[base_idx] |= (static_cast<uint32_t>(value & 3) << shift);
}

void KociembaSolver::generateCornersUDEdgesDepth3Table() {
  const int total = CORNERS_CLASS_MAX * UD_EDGES_MAX;
  const std::string fname = "phase2_prun";
  const std::string filepath = getPrecomputedPath(fname);

  std::ifstream file_in(filepath, std::ios::binary);
  if (file_in.good()) {
    file_in.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      cornersUDEdgesDepth3_.resize(total / 16);
      file.read(reinterpret_cast<char*>(cornersUDEdgesDepth3_.data()), cornersUDEdgesDepth3_.size() * sizeof(uint32_t));
      file.close();
    }
    return;
  }

  cornersUDEdgesDepth3_.assign(total / 16, 0xffffffffu);

  std::vector<unsigned short> c_sym(CORNERS_CLASS_MAX, 0);
  for (int i = 0; i < CORNERS_CLASS_MAX; ++i) {
    int rep = cornerRep_[i];
    EdgeCornerCube cc;
    KociembaSolver solver(cc);
    solver.coordCube.setCorners(rep);
    cc = solver.getCube();
    for (int s = 0; s < N_SYM_D4h; ++s) {
      EdgeCornerCube ss = symCubes_[s];
      ss.multiplyCorners(cc);
      ss.multiplyCorners(symCubes_[inverseSymIdx_[s]]);
      KociembaSolver temp_solver(ss);
      if (temp_solver.coordCube.getCorners() == rep) {
        c_sym[i] |= (1 << s);
      }
    }
  }

  int c_classidx = 0;
  int ud_edge = 0;
  setCornersUDEdgesDepth3(UD_EDGES_MAX * c_classidx + ud_edge, 0);
  int done = 1;
  int depth = 0;

  while (depth < 10) {
    int depth3 = depth % 3;
    int idx = 0;
    int mult = (depth > 9) ? 1 : 2;
    for (int c_classidx = 0; c_classidx < CORNERS_CLASS_MAX; ++c_classidx) {
      int ud_edge = 0;
      while (ud_edge < UD_EDGES_MAX) {
        if (idx % 16 == 0 && cornersUDEdgesDepth3_[idx / 16] == 0xffffffffu && ud_edge < UD_EDGES_MAX - 16) {
          ud_edge += 16;
          idx += 16;
          continue;
        }
        if (getCornersUDEdgesDepth3(idx) == depth3) {
          int corner = cornerRep_[c_classidx];
          for (int m : {0, 1, 2, 7, 10, 9, 10, 11, 13, 16}) {  // U1,U2,U3,R2,F2,D1,D2,D3,L2,B2
            int ud_edge1 = udEdgesMoveTable_[NUMBER_OF_MOVES * ud_edge + m];
            int corner1 = cornersMoveTable_[NUMBER_OF_MOVES * corner + m];
            int c1_classidx = cornerClassIdx_[corner1];
            int c1_sym = cornerSym_[corner1];
            ud_edge1 = udEdgesConjTable_[(ud_edge1 << 4) + c1_sym];
            int idx1 = UD_EDGES_MAX * c1_classidx + ud_edge1;
            if (getCornersUDEdgesDepth3(idx1) == 3) {
              setCornersUDEdgesDepth3(idx1, (depth + 1) % 3);
              done += 1;
              unsigned short sym = c_sym[c1_classidx];
              if (sym != 1) {
                for (int k = 1; k < 16; ++k) {
                  sym >>= 1;
                  if (sym % 2 == 1) {
                    int ud_edge2 = udEdgesConjTable_[(ud_edge1 << 4) + k];
                    int idx2 = UD_EDGES_MAX * c1_classidx + ud_edge2;
                    if (getCornersUDEdgesDepth3(idx2) == 3) {
                      setCornersUDEdgesDepth3(idx2, (depth + 1) % 3);
                      done += 1;
                    }
                  }
                }
              }
            }
          }
        }
        ud_edge += 1;
        idx += 1;
      }
    }
    depth += 1;
  }

  std::ofstream file_out(filepath, std::ios::binary);
  if (file_out.is_open()) {
    file_out.write(reinterpret_cast<const char*>(cornersUDEdgesDepth3_.data()),
                   cornersUDEdgesDepth3_.size() * sizeof(uint32_t));
    file_out.close();
  }
}

}  // namespace Kociemba