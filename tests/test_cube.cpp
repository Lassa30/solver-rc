#include <cube/cube.hpp>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>

#include <iostream>

using namespace Cube;

TEST_CASE("FrontendCube") {
  FrontendCube cube;
  CHECK(true);
}

TEST_CASE("EdgeCornerCube") {
  EdgeCornerCube cube{};
  cube.multiply(cube);
  bool identity = cube == EdgeCornerCube();
  CHECK(identity);
}
