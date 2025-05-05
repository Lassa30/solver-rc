#pragma once

#include <solver.hpp>

#include <array>
#include <iostream>
#include <memory>

/*
*********
* Enums *
*********
*/
enum class SideRotation {
  U, // Up
  D, // Down
  F, // Front
  B, // Back
  R, // Right
  L, // Left
};
enum class CubeRotation { LEFT, RIGHT, UP, DOWN };

enum class ColorID {
  FRONT = 0,
  BACK,
  RIGHT = 2,
  LEFT,
  UPPER = 4,
  BOTTOM,
  NONE = -1
};

/*
************************
* RubixCube interface *
************************
*/
class RubixCube {
private:
  bool isSolved_ = false;
  ColorID frontSide_ = ColorID::NONE;
  std::array<std::array<ColorID, 9>, 6> state_;

  std::unique_ptr<BaseSolver> solver_;

public:
  /*construct, copy, move, destruct*/
  RubixCube() = default;
  RubixCube(RubixCube &&) noexcept = default;
  RubixCube &operator=(RubixCube &&) noexcept = default;
  RubixCube(const RubixCube &) = delete;
  RubixCube &operator=(const RubixCube &) = delete;
  ~RubixCube() = default;

  /*checkers*/
  bool isValid();
  bool isSolved();

  bool operator==(const RubixCube &);
  bool operator!=(const RubixCube &);

  /*setters*/
  void setState(std::array<std::array<ColorID, 9>, 6> state);
  void setSolver(std::unique_ptr<BaseSolver> &solver);
  void setColor(ColorID color, int index);

  /*getters*/
  const std::array<std::array<ColorID, 9>, 6> &getState() const;
  const std::unique_ptr<BaseSolver> &getSolver() noexcept;
  const ColorID &getColor(int index) const;

  /*main methods*/
  SolverStatus solve();
  void rotateSide(SideRotation rotation, int repeat);
  void rotateCube(CubeRotation rotation, int repeat);

private:
  /*helper functions for cube validation*/
  bool checkCornerOrientation();
  bool checkEdgeOrientation();
  bool checkPermutationParity();
  void genValidationReport(std::string &out);

  /*side rotations*/
  void rotateUpper(int repeat);
  void rotateBottom(int repeat);

  void rotateRight(int repeat);
  void rotateLeft(int repeat);

  void rotateFront(int repeat);
  void rotateDown(int repeat);

  /*changing facing side*/
  void faceLeft(int repeat);
  void faceRight(int repeat);
  void faceUp(int repeat);
  void faceDown(int repeat);
};