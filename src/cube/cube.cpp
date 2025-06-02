#include "cube.hpp"
#include <cube/cube.hpp>

#include <algorithm>
#include <iostream>

namespace Cube {
/*static data definitions*/
const std::array<Corner, 8> EdgeCornerCube::cornersSolved{{{CornerID::URF, 0},
                                                           {CornerID::UFL, 0},
                                                           {CornerID::ULB, 0},
                                                           {CornerID::UBR, 0},
                                                           {CornerID::DFR, 0},
                                                           {CornerID::DLF, 0},
                                                           {CornerID::DBL, 0},
                                                           {CornerID::DRB, 0}}};

const std::array<Edge, 12> EdgeCornerCube::edgesSolved{{EdgeID::UR, 0, EdgeID::UF, 0, EdgeID::UL, 0, EdgeID::UB, 0,
                                                        EdgeID::DR, 0, EdgeID::DF, 0, EdgeID::DL, 0, EdgeID::DB, 0,
                                                        EdgeID::FR, 0, EdgeID::FL, 0, EdgeID::BL, 0, EdgeID::BR, 0}};

void EdgeCornerCube::generateMoveCubes() {
  for (int cid = 0; cid < static_cast<int>(ColorID::COUNT); cid++) {
    auto tempCube = EdgeCornerCube();

    for (int count = 0; count < 3; count++) {
      tempCube.multiply(basicMovesCubes[cid]);
      moveCubes_[3 * cid + count] = tempCube;
    }
  }
}

/*static member initialization with lambda function*/
std::array<EdgeCornerCube, 18> EdgeCornerCube::moveCubes_;
bool EdgeCornerCube::isMoveCubesGenerated_ = [] {
  EdgeCornerCube::generateMoveCubes();
  return true;
}();

/*EdgeCornerCube - operators*/
bool Edge::operator==(const Edge &rhs) const { return orientation == rhs.orientation && position == rhs.position; }
bool Edge::operator!=(const Edge &rhs) const { return !operator==(rhs); }

bool Corner::operator==(const Corner &rhs) const { return orientation == rhs.orientation && position == rhs.position; }
bool Corner::operator!=(const Corner &rhs) const { return !operator==(rhs); }

bool EdgeCornerCube::operator==(const EdgeCornerCube &rhs) const {
  return std::equal(corners_.begin(), corners_.end(), rhs.corners_.begin()) &&
         std::equal(edges_.begin(), edges_.end(), rhs.edges_.begin());
}

bool EdgeCornerCube::operator!=(const EdgeCornerCube &rhs) const { return !operator==(rhs); }

/*EdgeCornerCube - constructors*/
EdgeCornerCube::EdgeCornerCube(const std::array<EdgeID, 12> &ep, const std::array<int, 12> &eo,
                               const std::array<CornerID, 8> &cp, const std::array<int, 8> &co) {
  auto edges = edges_;
  auto corners = corners_;
  for (int edgeIdx = 0; edgeIdx < static_cast<int>(EdgeID::COUNT); edgeIdx++) {
    auto &[pos, ori] = edges[edgeIdx];
    pos = ep[edgeIdx];
    ori = eo[edgeIdx];
  }
  for (int cornerIdx = 0; cornerIdx < static_cast<int>(CornerID::COUNT); cornerIdx++) {
    auto &[pos, ori] = corners[cornerIdx];
    pos = cp[cornerIdx];
    ori = co[cornerIdx];
  }
  corners_ = std::move(corners);
  edges_ = std::move(edges);
}

EdgeCornerCube::EdgeCornerCube(const std::array<Edge, 12> &edges, const std::array<Corner, 8> &corners)
    : edges_{edges}, corners_{corners} {}

EdgeCornerCube::EdgeCornerCube(const EdgeCornerCube &cube) : edges_{cube.edges_}, corners_{cube.corners_} {}

EdgeCornerCube &EdgeCornerCube::operator=(const EdgeCornerCube &rhs) {
  if (&rhs != this) {
    edges_ = rhs.edges_;
    corners_ = rhs.corners_;
  }
  return *this;
}

/*EdgeCornerCube main interface - movements and permutation product*/

int EdgeCornerCube::calculateOrientation(int ori_lhs, int ori_rhs) {
  int ori = 0;
  if (ori_lhs < 3 && ori_rhs < 3) {
    ori = ori_lhs + ori_rhs;
    ori = (ori >= 3) ? ori - 3 : ori;
  } else if (ori_lhs < 3 && ori_rhs >= 3) {
    ori = ori_lhs + ori_rhs;
    ori = (ori >= 6) ? ori - 3 : ori;
  } else if (ori_lhs >= 3 && ori_rhs < 3) {
    ori = ori_lhs - ori_rhs;
    ori = (ori < 3) ? ori + 3 : ori;
  } else if (ori_lhs >= 3 && ori_rhs >= 3) {
    ori = ori_lhs - ori_rhs;
    ori = (ori < 0) ? ori + 3 : ori;
  }
  return ori;
}

void EdgeCornerCube::multiplyCorners(const EdgeCornerCube &rhs) {
  std::array<Corner, 8> corners;
  for (int cornerId = 0; cornerId < (int)CornerID::COUNT; cornerId++) {
    auto cornerIdx = static_cast<int>(rhs.corners_[cornerId].position);

    int orientation_lhs = corners_[cornerIdx].orientation;
    int orientation_rhs = rhs.corners_[cornerId].orientation;
    corners[cornerId].position = corners_[cornerIdx].position;
    corners[cornerId].orientation = calculateOrientation(orientation_lhs, orientation_rhs);
  }
  corners_ = corners;
}

void EdgeCornerCube::multiplyEdges(const EdgeCornerCube &rhs) {
  std::array<Edge, 12> edges;
  for (int e = 0; e < (int)EdgeID::COUNT; e++) {
    int edgeIdx = static_cast<int>(rhs.edges_[e].position);
    edges[e].position = edges_[edgeIdx].position;
    edges[e].orientation = (rhs.edges_[e].orientation + edges_[edgeIdx].orientation) % 2;
  }
  edges_ = edges;
}

void EdgeCornerCube::multiply(const EdgeCornerCube &rhs) {
  multiplyCorners(rhs);
  multiplyEdges(rhs);
}

void EdgeCornerCube::applyMove(SideMovement move) { multiply(EdgeCornerCube::getMoveCube(move)); }

/*EdgeCornerCube misc - functions for string representation and validity checking*/
std::string EdgeCornerCube::toString() const {
  std::stringstream strStream;

  for (int i = 0; i < static_cast<int>(CornerID::COUNT); ++i) {
    strStream << '(' << static_cast<int>(corners_[i].position) << ',' << corners_[i].orientation << ')';
  }
  strStream << '\n';
  for (int i = 0; i < static_cast<int>(EdgeID::COUNT); ++i) {
    strStream << '(' << static_cast<int>(edges_[i].position) << ',' << edges_[i].orientation << ')';
  }
  return strStream.str();
}

int EdgeCornerCube::cornerParity() const {
  int parity = 0;
  for (int i = int(CornerID::DRB); i > int(CornerID::URF); i--) {
    for (int j = i - 1; j > int(CornerID::URF) - 1; j--) {
      if (corners_[j].position > corners_[i].position) {
        parity += 1;
      }
    }
  }
  return parity % 2;
}

int EdgeCornerCube::edgeParity() const {
  int parity = 0;
  for (int i = int(EdgeID::BR); i > int(EdgeID::UR); i--) {
    for (int j = i - 1; j > int(EdgeID::UR) - 1; j--) {
      if (edges_[j].position > edges_[i].position) {
        parity += 1;
      }
    }
  }
  return parity % 2;
}

int EdgeCornerCube::fromString(const std::string &str) {
  /*basic checks*/
  if (str.size() != 54) {
    return 1;
  }
  std::array<(int)ColorID::COUNT> colorsCounter = {0, 0, 0, 0, 0, 0};
  bool hasInvalid = false;
  for (const auto c & : str) {
    switch (c) {
      case 'U':
        colorsCounter[(int)ColorID::U] += 1;
        break;
      case 'R':
        colorsCounter[(int)ColorID::R] += 1;
        break;
      case 'F':
        colorsCounter[(int)ColorID::F] += 1;
        break;
      case 'D':
        colorsCounter[(int)ColorID::D] += 1;
        break;
      case 'L':
        colorsCounter[(int)ColorID::L] += 1;
        break;
      case 'B':
        colorsCounter[(int)ColorID::B] += 1;
        break;
      default:
        hasInvalid = true;
        break;
    }
    (c)
  }
  if (hasInvalid) {
    return 2;
  }

  bool allCountersAreNine =
      std::all_of(colorsCounter.begin(), colorsCounter.end(), [](int counter) { return counter == 9; });
  if (!allCountersAreNine) {
    return 3;
  }

  /*edges and corners*/
  cube = EdgeCornerCube();
  const std::array<int, 8> cornerPos;
  const std::array<int, 12> edgePos;
  for (int cornerIdx = 0; cornerIdx < (int)CornerID::COUNT; cornerIdx++) { 
  }

  /*final*/
  *this = EdgeCornerCube(edges, corners);
  return 0;
}

}  // namespace Cube