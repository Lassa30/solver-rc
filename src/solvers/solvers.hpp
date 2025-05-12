#pragma once

#include <cube/cube.hpp>

#include <array>
#include <bitset>
#include <string>

using namespace Cube;

/* Kociemba's algorithm: two-phase algorithm */
class KociembaSolver {
public:
  explicit KociembaSolver(const Cube::EdgeCornerCube&);

  const EdgeCornerCube& getCube() const;

  /*symmetry related things for speeding up the process*/
  EdgeCornerCube inverseCube() const;
  std::bitset<96> symmetriesCube() const;

  /*symmetry checkers*/
  bool hasRotationalSymmetry(const std::bitset<96>&) const;
  bool hasAntiSymmetry(const std::bitset<96>&) const;

  /*main solve function: idk the interface yet*/
  std::string solve();

private:
  class CoordinateCube;
  EdgeCornerCube ecCube_;

  /*static members for cube symmetries*/
  static std::array<EdgeCornerCube, static_cast<int>(BasicSymmetry::COUNT)> basicSymCubes_;
  static std::array<EdgeCornerCube, NUMBER_OF_SYMMETRIES> symCubes_;
  static std::array<int, NUMBER_OF_SYMMETRIES> inverseSymIdx_;

  static bool isBasicSymCubesGenerated_;
  static bool isSymCubesGenerated_;
  static bool isInverseSymIdxGenerated_;

  static void generateBasicSymCubes();
  static void generateSymCubes();
  static void generateInverseSymIdx();
};
