#pragma once

#include <string>

class BaseSolver {
public:
  virtual std::string solve() = 0;
};

/* Kociemba’s algorithm: two-phase algorithm */
class KociembaSolver : public BaseSolver {
public:
  std::string solve() override;
};

/* Custom algorithm: view README.md */
class CustomSolver : public BaseSolver {
public:
  std::string solve() override;
};

/* Thistlethwaite’s algorithm: 52 moves algorithm */
class ThistlethwaiteSolver : public BaseSolver {
public:
  std::string solve() override;
};
