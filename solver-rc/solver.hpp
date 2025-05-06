#pragma once

#include <string>

class RubixCube;

class BaseSolver {
public:
  virtual void adaptRepresentation(const RubixCube &) = 0;
  virtual std::string solve() = 0;
};

/* Kociemba’s algorithm: two-phase algorithm */
class KociembaSolver : public BaseSolver {
public:
  void adaptRepresentation(const RubixCube &) override;
  std::string solve() override;
};

/* Custom algorithm: view README.md */
class CustomSolver : public BaseSolver {
public:
  void adaptRepresentation(const RubixCube &) override;
  std::string solve() override;
};

/* Thistlethwaite’s algorithm: 52 steps algorithm */
class ThistlethwaiteSolver : public BaseSolver {
public:
  void adaptRepresentation(const RubixCube &) override;
  std::string solve() override;
};
