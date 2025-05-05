#pragma once

#include <cube.hpp>

enum class SolverStatus { SUCCESS = 0, INVALID, INVALID_TARGET, FAILED = -1 };

class BaseSolver {
public:
  virtual void represent(const RubixCube &) = 0;
  virtual SolverStatus solve() = 0;

};