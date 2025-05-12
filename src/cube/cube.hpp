#pragma once

#include <cube/constants.hpp>

#include <array>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace Cube {
class EdgeCornerCube;

/*
*******************************************************************************
* FrontendCube: used inside GUI, for color calibration, OpenGL                *
*******************************************************************************
*/

class FrontendCube {
private:
  std::array<ColorID, 54> state_;

public:
  /*construct, copy, move, destruct*/
  FrontendCube() = default;
  FrontendCube(FrontendCube &&) noexcept = default;
  FrontendCube &operator=(FrontendCube &&) noexcept = default;

  FrontendCube(const FrontendCube &) = default;
  FrontendCube &operator=(const FrontendCube &) = default;
  ~FrontendCube() = default;

  /*I don't know the interface yet...*/
};

/*
*******************************************************************************
* EdgeCornerCube: is going to be used by solvers (possibly with adaptors)     *
*******************************************************************************
*/

struct Edge {
  EdgeID position;
  int orientation;
  bool operator==(const Edge &) const;
  bool operator!=(const Edge &) const;
};

struct Corner {
  CornerID position;
  int orientation;
  bool operator==(const Corner &) const;
  bool operator!=(const Corner &) const;
};

class EdgeCornerCube {
public:
  EdgeCornerCube() = default;
  EdgeCornerCube(const std::array<Edge, 12> &edges, const std::array<Corner, 8> &corners);
  EdgeCornerCube(const std::array<EdgeID, 12> &, const std::array<int, 12> &, const std::array<CornerID, 8> &,
                 const std::array<int, 8> &);

  void applyMove(SideMovement move);
  void multiply(const EdgeCornerCube &rhs);

  bool operator==(const EdgeCornerCube &) const;
  bool operator!=(const EdgeCornerCube &) const;

  inline const Corner& getCorner(int cornerIdx) const { return corners_.at(cornerIdx); }
  inline const Edge& getEdge(int edgeIdx) const { return edges_.at(edgeIdx); }

  /*implement later if required*/
  // bool isValidCube() const;

  std::string toString() const;

  static inline const EdgeCornerCube &getMoveCube(SideMovement move) { return moveCubes_[(int)move]; }

private:
  /*initial state is the solved cube*/
  std::array<Corner, 8> corners_ = cornersSolved;
  std::array<Edge, 12> edges_ = edgesSolved;

  /*functions to implement multiplication*/
  int calculateOrientation(int, int);
  void multiplyCorners(const EdgeCornerCube &rhs);
  void multiplyEdges(const EdgeCornerCube &rhs);

  /*helpers to implement EdgeCornerCube::isValid*/

  int edgeParity() const;
  int cornerParity() const;

  /*data and functions shared across all EdgeCornerCubes*/
  static const std::array<Corner, 8> cornersSolved;
  static const std::array<Edge, 12> edgesSolved;

  static bool isMoveCubesGenerated_;
  static std::array<EdgeCornerCube, 18> moveCubes_;

  /*moveCubes_ initializer*/
  static void generateMoveCubes();
};

/*basic cube moves*/
static const auto cubeMoveUp = EdgeCornerCube(edgePositionU, edgeOrientationU, cornerPositionU, cornerOrientationU);
static const auto cubeMoveRight = EdgeCornerCube(edgePositionR, edgeOrientationR, cornerPositionR, cornerOrientationR);
static const auto cubeMoveFront = EdgeCornerCube(edgePositionF, edgeOrientationF, cornerPositionF, cornerOrientationF);
static const auto cubeMoveDown = EdgeCornerCube(edgePositionD, edgeOrientationD, cornerPositionD, cornerOrientationD);
static const auto cubeMoveLeft = EdgeCornerCube(edgePositionL, edgeOrientationL, cornerPositionL, cornerOrientationL);
static const auto cubeMoveBack = EdgeCornerCube(edgePositionB, edgeOrientationB, cornerPositionB, cornerOrientationB);

static const std::array<EdgeCornerCube, 6> basicMovesCubes = {cubeMoveUp,   cubeMoveRight, cubeMoveFront,
                                                              cubeMoveDown, cubeMoveLeft,  cubeMoveBack};

}  // namespace Cube