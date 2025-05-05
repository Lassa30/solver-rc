#pragma once

#include <cube.hpp>
#include <string>

class RubixCube;
// forward declaration is required

class BaseSolver {
public:
  virtual void adaptRepresentation(const RubixCube &) = 0;
  virtual std::string solve() = 0;
};

/* Kociemba’s algorithm: two-phase algorithm */
class KociembaSolver : BaseSolver {
public:
  void adaptRepresentation(const RubixCube &) override;
  std::string solve() override;
};

/* Custom algorithm: view README.md */
class CustomSolver : BaseSolver {
public:
  void adaptRepresentation(const RubixCube &) override;
  std::string solve() override;
};

/* Thistlethwaite’s algorithm */
class ThistlethwaiteSolver : BaseSolver {
public:
  void adaptRepresentation(const RubixCube &) override;
  std::string solve() override;
};

/* Human-like solving method, implemented by hard-coding the following steps:
CFOP:
1) C_ross: complete white cross
2) F_2L: first two layers
3) O_LL: orient last layer
4) PLL: permute last layer
*/
class HumanLikeSolver : BaseSolver {
public:
  void adaptRepresentation(const RubixCube &) override;
  std::string solve() override;
};
