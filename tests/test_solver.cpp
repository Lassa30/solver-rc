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
  auto solver = Kociemba::KociembaSolver{cube};
  std::string cubeString;

  auto invCube = solver.inverseCube();
  CHECK((invCube == cube));

  // Checking inverse cube after applying some moves
  applyDefaultMoves(cube);

  CHECK(cube.toString() ==
        "(1,2)(0,1)(7,0)(5,1)(3,2)(4,1)(2,0)(6,2)\n"
        "(11,0)(1,1)(4,0)(0,0)(8,0)(10,1)(3,0)(5,0)(7,1)(2,1)(9,0)(6,0)");

  solver = Kociemba::KociembaSolver{cube};
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
  auto solver = Kociemba::KociembaSolver{cube};
  auto symmetries = solver.symmetriesCube();

  std::bitset<96> allSymmetries;
  allSymmetries.set();
  CHECK(symmetries == allSymmetries);

  // Apply the default moves sequence
  applyDefaultMoves(cube);

  // After moves, only symmetry 0 should remain
  solver = Kociemba::KociembaSolver{cube};
  symmetries = solver.symmetriesCube();

  std::bitset<96> onlyIdentitySymmetry{0};
  onlyIdentitySymmetry.set(0, 1);
  CHECK(symmetries == onlyIdentitySymmetry);

  // Check rotational symmetry and antisymmetry
  CHECK_FALSE(solver.hasRotationalSymmetry(symmetries));
  CHECK_FALSE(solver.hasAntiSymmetry(symmetries));
}

TEST_CASE("Phase 1 Coordinate mappings") {
  SUBCASE("Solved cube") {
    EdgeCornerCube cube{};
    auto solver = Kociemba::KociembaSolver{cube};
    // ---Getters for solved state first---
    auto twist = solver.coordCube.getTwist();
    auto flip = solver.coordCube.getFlip();
    auto slice_sorted = solver.coordCube.getSliceSorted();

    auto SOLVED = Kociemba::KociembaSolver::CoordinateCube::SOLVED;
    CHECK(twist == SOLVED);
    CHECK(flip == SOLVED);
    CHECK(slice_sorted == SOLVED);

    // ---Setting some cube state and checking it---
    solver.coordCube.setTwist(42);
    solver.coordCube.setFlip(42);
    solver.coordCube.setSliceSorted(42);

    CHECK(solver.coordCube.getTwist() == 42);
    CHECK(solver.coordCube.getFlip() == 42);
    CHECK(solver.coordCube.getSliceSorted() == 42);

    CHECK(solver.getCube().toString() ==
          "(0,0)(1,0)(2,0)(3,1)(4,1)(5,2)(6,0)(7,2)\n"
          "(0,0)(1,0)(2,0)(3,0)(4,0)(5,1)(6,0)(9,1)(7,0)(10,1)(11,0)(8,1)");
  }

  SUBCASE("Apply moves and check coordinates") {
    EdgeCornerCube cube{};
    applyDefaultMoves(cube);
    auto solver = Kociemba::KociembaSolver{cube};

    auto twist = solver.coordCube.getTwist();
    auto flip = solver.coordCube.getFlip();
    auto slice_sorted = solver.coordCube.getSliceSorted();
    // ---Basic Check---
    CHECK(twist == 1749);
    CHECK(flip == 550);
    CHECK(slice_sorted == 9155);
  }
}

TEST_CASE("Phase 2 Coordinate mappings") {
  SUBCASE("Apply moves and check coordinates") {
    EdgeCornerCube cube{};
    applyDefaultMoves(cube);
    auto solver = Kociemba::KociembaSolver{cube};

    auto u_edges = solver.coordCube.getUEdges();
    auto d_edges = solver.coordCube.getDEdges();
    auto corners = solver.coordCube.getCorners();
    auto ud_edges = solver.coordCube.getUDEdges();

    auto slice_sorted = solver.coordCube.getSliceSorted();

    CHECK(u_edges == 5691);
    CHECK(d_edges == 9512);
    CHECK(corners == 19131);
    CHECK(slice_sorted == 9155);
    CHECK(ud_edges == -1);  // -1 because slice_sorted >= 24
  }
}
