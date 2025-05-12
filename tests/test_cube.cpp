#include <cube/cube.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <iostream>

using namespace Cube;

void CHECK_MOVE(EdgeCornerCube &, SideMovement, const std::string &);

TEST_CASE("EdgeCornerCube movements") {
  // Herber Kociemba's implementation results are taken as a reference

  EdgeCornerCube cube{};
  cube.multiply(cube);

  bool identity = (cube == EdgeCornerCube());
  CHECK(identity);

  CHECK_MOVE(cube, SideMovement::U,
             "(3,0)(0,0)(1,0)(2,0)(4,0)(5,0)(6,0)(7,0)\n"
             "(3,0)(0,0)(1,0)(2,0)(4,0)(5,0)(6,0)(7,0)(8,0)(9,0)(10,0)(11,0)");

  auto cubeCopy = cube;
  CHECK((cubeCopy == cube));

  // Doing D3 twice
  CHECK_MOVE(cube, SideMovement::D3,
             "(3,0)(0,0)(1,0)(2,0)(7,0)(4,0)(5,0)(6,0)\n"
             "(3,0)(0,0)(1,0)(2,0)(7,0)(4,0)(5,0)(6,0)(8,0)(9,0)(10,0)(11,0)");
  cube.applyMove(SideMovement::D3);

  // Doing D2 once
  CHECK_MOVE(cubeCopy, SideMovement::D2,
             "(3,0)(0,0)(1,0)(2,0)(6,0)(7,0)(4,0)(5,0)\n"
             "(3,0)(0,0)(1,0)(2,0)(6,0)(7,0)(4,0)(5,0)(8,0)(9,0)(10,0)(11,0)");

  // The result should be the same
  CHECK((cube == cubeCopy));

  // The final move here: B2
  CHECK_MOVE(cube, SideMovement::B2,
             "(3,0)(0,0)(5,0)(4,0)(6,0)(7,0)(2,0)(1,0)\n"
             "(3,0)(0,0)(1,0)(5,0)(6,0)(7,0)(4,0)(2,0)(8,0)(9,0)(11,0)(10,0)");
}

TEST_CASE("EdgeCornerCube coordinates") {}

/*helper functions definitions*/
void CHECK_MOVE(EdgeCornerCube &cube, SideMovement move, const std::string &expected) {
  cube.applyMove(move);
  auto result = cube.toString();
  CHECK(result == expected);
}
