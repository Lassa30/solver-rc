#pragma once

#include <cube/constants.hpp>

#include <array>
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
};

struct Corner {
  CornerID position;
  int orientation;
};

class EdgeCornerCube {
public:
  EdgeCornerCube() = default;
  EdgeCornerCube(const std::array<Edge, 12> &edges,
                 const std::array<Corner, 8> &corners);
  EdgeCornerCube(const std::array<EdgeID, 12> &, const std::array<int, 12> &,
                 const std::array<CornerID, 8> &, const std::array<int, 8> &);

  void multiply(const EdgeCornerCube &rhs);

  bool operator==(const EdgeCornerCube &);
  bool operator!=(const EdgeCornerCube &);

  EdgeCornerCube inverseCube();
  std::vector<EdgeCornerCube> symmetries();

  FrontendCube toFrontendCube() const;

private:
  /*initial state is the solved cube*/
  std::array<Corner, 8> corners_ = cornersSolved;
  std::array<Edge, 12> edges_ = edgesSolved;

  void multiplyCorners(const EdgeCornerCube &rhs);
  void multiplyEdges(const EdgeCornerCube &rhs);

  int edgeParity();
  int cornerParity();
  bool cubeIsValid();
  void generateMoveCubes();

  int calculateOrientation(int, int);

private:
private:
  static bool isMoveCubesGenerated_;
  static const std::array<Corner, 8> cornersSolved;
  static const std::array<Edge, 12> edgesSolved;
};

/*basic cube moves*/
static const auto cubeMoveUp = EdgeCornerCube(
    edgePositionU, edgeOrientationU, cornerPositionU, cornerOrientationU);
static const auto cubeMoveRight = EdgeCornerCube(
    edgePositionR, edgeOrientationR, cornerPositionR, cornerOrientationR);
static const auto cubeMoveFront = EdgeCornerCube(
    edgePositionF, edgeOrientationF, cornerPositionF, cornerOrientationF);
static const auto cubeMoveDown = EdgeCornerCube(
    edgePositionD, edgeOrientationD, cornerPositionD, cornerOrientationD);
static const auto cubeMoveLeft = EdgeCornerCube(
    edgePositionL, edgeOrientationL, cornerPositionL, cornerOrientationL);
static const auto cubeMoveBack = EdgeCornerCube(
    edgePositionB, edgeOrientationB, cornerPositionB, cornerOrientationB);
static std::array<EdgeCornerCube, 18> moveCubes_;

} // namespace Cube