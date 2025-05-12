#include <cube/cube.hpp>
#include <solvers/solvers.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <iostream>
#include <string>

using namespace Cube;

static constexpr std::array<SideMovement, 5> defaultMoves = {SideMovement::U3, SideMovement::D2, SideMovement::R3,
                                                             SideMovement::L2, SideMovement::F3};
void applyDefaultMoves(EdgeCornerCube& cube) {
  for (auto move : defaultMoves) {
    cube.applyMove(move);
  }
}

TEST_CASE("Inverse") {
  // Trivial case: inversed solved cube is the same as just solved cube
  auto cube = EdgeCornerCube{};
  auto solver = KociembaSolver{cube};
  std::string cubeString;

  auto invCube = solver.inverseCube();
  CHECK((invCube == cube));

  // Checking inverse cube after applying some moves
  applyDefaultMoves(cube);

  CHECK(cube.toString() ==
        "(1,2)(0,1)(7,0)(5,1)(3,2)(4,1)(2,0)(6,2)\n"
        "(11,0)(1,1)(4,0)(0,0)(8,0)(10,1)(3,0)(5,0)(7,1)(2,1)(9,0)(6,0)");

  solver = KociembaSolver{cube};
  invCube = solver.inverseCube();
  cubeString = invCube.toString();
  std::string expectedString = "";

  CHECK(invCube.toString() ==
        "(1,2)(0,1)(6,0)(4,1)(5,2)(3,2)(7,1)(2,0)\n"
        "(3,0)(1,1)(9,1)(6,0)(2,0)(7,0)(11,0)(8,1)(4,0)(10,0)(5,1)(0,0)");
}

TEST_CASE("Symmetries") {
  // Test with solved cube - should have all 96 symmetries
  auto cube = EdgeCornerCube{};
  auto solver = KociembaSolver{cube};
  auto symmetries = solver.symmetriesCube();

  std::bitset<96> allSymmetries;
  allSymmetries.set();
  CHECK(symmetries == allSymmetries);

  // Apply the default moves sequence
  applyDefaultMoves(cube);

  // After moves, only symmetry 0 should remain
  solver = KociembaSolver{cube};
  symmetries = solver.symmetriesCube();

  std::bitset<96> onlyIdentitySymmetry{0};
  onlyIdentitySymmetry.set(0, 1);
  CHECK(symmetries == onlyIdentitySymmetry);

  // Check rotational symmetry and antisymmetry
  CHECK_FALSE(solver.hasRotationalSymmetry(symmetries));
  CHECK_FALSE(solver.hasAntiSymmetry(symmetries));
}
