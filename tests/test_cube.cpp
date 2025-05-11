#include <cube/cube.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <iostream>

using namespace Cube;

void CHECK_MOVE(EdgeCornerCube &, SideMovement, const std::string &);

TEST_CASE("EdgeCornerCube movements") {

  SUBCASE("Solved * Solved == Solved") {
    EdgeCornerCube cube{};
    cube.multiply(cube);

    bool identity = (cube == EdgeCornerCube());
    CHECK(identity);
  }

  // tests:
  //   1) U
  //   2) D3+D3 == D2
  //   3) B2
  // kociemba's implementation results are taken as a reference

  EdgeCornerCube cube{};
  std::string cubeStr = "";
  SUBCASE("Just `U`") {
    CHECK_MOVE(
        cube, SideMovement::U,
        "(3,0)(0,0)(1,0)(2,0)(4,0)(5,0)(6,0)(7,0)\n"
        "(3,0)(0,0)(1,0)(2,0)(4,0)(5,0)(6,0)(7,0)(8,0)(9,0)(10,0)(11,0)");
  }
  SUBCASE("D3 + D3 == D2 AND D3 + D3 + B2") {
    auto cubeCopy = cube;
    CHECK((cubeCopy == cube));

    // Doing D3 twice
    CHECK_MOVE(
        cube, SideMovement::D3,
        "(0,0)(1,0)(2,0)(3,0)(7,0)(4,0)(5,0)(6,0)\n"
        "(0,0)(1,0)(2,0)(3,0)(7,0)(4,0)(5,0)(6,0)(8,0)(9,0)(10,0)(11,0)");
    cube.applyMove(SideMovement::D3);

    // Doing D2 once
    CHECK_MOVE(
        cubeCopy, SideMovement::D2,
        "(0,0)(1,0)(2,0)(3,0)(6,0)(7,0)(4,0)(5,0)\n"
        "(0,0)(1,0)(2,0)(3,0)(6,0)(7,0)(4,0)(5,0)(8,0)(9,0)(10,0)(11,0)");

    // The result should be the same
    CHECK((cube == cubeCopy));

    CHECK_MOVE(
        cube, SideMovement::B2,
        "(0,0)(1,0)(5,0)(4,0)(6,0)(7,0)(3,0)(2,0)\n"
        "(0,0)(1,0)(2,0)(5,0)(6,0)(7,0)(4,0)(3,0)(8,0)(9,0)(11,0)(10,0)");
  }
}

TEST_CASE("FrontendCube") { FrontendCube cube; }

/*helper functions definitions*/
void CHECK_MOVE(EdgeCornerCube &cube, SideMovement move,
                const std::string &expected) {
  cube.applyMove(move);
  auto result = cube.toString();
  CHECK(result == expected);
}
