#pragma once

#include <cube/cube.hpp>

#include <bitset>
#include <string>

/* Kociemba’s algorithm: two-phase algorithm */
class KociembaSolver {
public:
  explicit KociembaSolver(const Cube::EdgeCornerCube&);

private:
  class CoordinateCube;
  Cube::EdgeCornerCube edgeCornerCube_;

  /*symmetry related things for speeding up the process*/
  Cube::EdgeCornerCube inverseEdgeCornerCube() const;
  std::bitset<96> symmetriesEdgeCornerCube() const;

  /*symmetry checkers*/
  bool hasRotationalSymmetry(const std::bitset<96>&) const;
  bool hasAntiSymmetry(const std::bitset<96>&) const;

  /*main solve function: idk the interface yet*/
  std::string solve();
};
