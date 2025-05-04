#pragma once

#include <solver.hpp>

#include <array>
#include <iostream>
#include <memory>

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

class RubiksCube {
private:
  bool isSolved_ = false;
  ColorID frontSide_ = ColorID::NONE;
  std::array<std::array<ColorID, 9>, 6> state_;

  std::unique_ptr<BaseSolver> solver_;

public:
  /*construct, copy, move, destruct*/
  RubiksCube() = default;
  RubiksCube(RubiksCube &&) noexcept = default;
  RubiksCube &operator=(RubiksCube &&) noexcept = default;
  RubiksCube(const RubiksCube &) = delete;
  RubiksCube &operator=(const RubiksCube &) = delete;
  ~RubiksCube() = default;

  /*checkers*/
  bool isValid();
  bool isSolved();

  bool operator==(const RubiksCube &);
  bool operator!=(const RubiksCube &);

  /*setters*/
  void setState(std::array<std::array<ColorID, 9>, 6> state);
  void setSolver(std::unique_ptr<BaseSolver> &solver);
  void setColor(ColorID color, int index);

  /*getters*/
  const std::array<std::array<ColorID, 9>, 6> &getState();
  const std::unique_ptr<BaseSolver> &getSolver() noexcept;
  const ColorID &getColor(int index);

  /*main methods*/
  void rotateSide(SideRotation rotation, int repeat);
  void rotateCube(CubeRotation rotation, int repeat);

  SolverStatus solve();

private:
  bool isBalanced();
  void genValidationReport(std::string &out);

  /*side rotations*/
  void rotateUpper(int repeat);
  void rotateBottom(int repeat);

  void rotateRight(int repeat);
  void rotateLeft(int repeat);

  void rotateFront(int repeat);
  void rotateDown(int repeat);
};