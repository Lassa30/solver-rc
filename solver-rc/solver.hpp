#pragma once

enum class SolverStatus { SUCCESS = 0, INVALID, INVALID_TARGET, FAILED = -1 };

class BaseSolver {
public:
  SolverStatus solve();
};