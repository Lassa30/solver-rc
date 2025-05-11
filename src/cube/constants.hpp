#pragma once

#include <cube/cube.hpp>
#include <cube/enums.hpp>

#include <array>
#include <string>

namespace Cube {
// clang-format off

/*cube moves are defined as positions and orientations*/

// Up move
static constexpr std::array<CornerID, 8> cornerPositionU{
  CornerID::UBR, CornerID::URF, CornerID::UFL, CornerID::ULB,
  CornerID::DFR, CornerID::DLF, CornerID::DBL, CornerID::DRB};
static constexpr std::array<int, 8> cornerOrientationU{0, 0, 0, 0,
                                                     0, 0, 0, 0};
static constexpr std::array<EdgeID, 12> edgePositionU{
  EdgeID::UB, EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::DR, EdgeID::DF,
  EdgeID::DL, EdgeID::DB, EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};
static constexpr std::array<int, 12> edgeOrientationU{0, 0, 0, 0, 0, 0,
                                                    0, 0, 0, 0, 0, 0};

// Right move
static constexpr std::array<CornerID, 8> cornerPositionR{
  CornerID::DFR, CornerID::UFL, CornerID::ULB, CornerID::URF,
  CornerID::DRB, CornerID::DLF, CornerID::DBL, CornerID::UBR};
static constexpr std::array<int, 8> cornerOrientationR{2, 0, 0, 1,
                                                     1, 0, 0, 2};
static constexpr std::array<EdgeID, 12> edgePositionR{
  EdgeID::FR, EdgeID::UF, EdgeID::UL, EdgeID::UB, EdgeID::BR, EdgeID::DF,
  EdgeID::DL, EdgeID::DB, EdgeID::DR, EdgeID::FL, EdgeID::BL, EdgeID::UR};
static constexpr std::array<int, 12> edgeOrientationR{0, 0, 0, 0, 0, 0,
                                                    0, 0, 0, 0, 0, 0};

// Front move
static constexpr std::array<CornerID, 8> cornerPositionF{
  CornerID::UFL, CornerID::DLF, CornerID::ULB, CornerID::UBR,
  CornerID::URF, CornerID::DFR, CornerID::DBL, CornerID::DRB};
static constexpr std::array<int, 8> cornerOrientationF{1, 2, 0, 0,
                                                     2, 1, 0, 0};
static constexpr std::array<EdgeID, 12> edgePositionF{
  EdgeID::UR, EdgeID::FL, EdgeID::UL, EdgeID::UB, EdgeID::DR, EdgeID::FR,
  EdgeID::DL, EdgeID::DB, EdgeID::UF, EdgeID::DF, EdgeID::BL, EdgeID::BR};
static constexpr std::array<int, 12> edgeOrientationF{0, 1, 0, 0, 0, 1,
                                                    0, 0, 1, 1, 0, 0};

// Down move
static constexpr std::array<CornerID, 8> cornerPositionD{
  CornerID::URF, CornerID::UFL, CornerID::ULB, CornerID::UBR,
  CornerID::DLF, CornerID::DBL, CornerID::DRB, CornerID::DFR};
static constexpr std::array<int, 8> cornerOrientationD{0, 0, 0, 0,
                                                     0, 0, 0, 0};
static constexpr std::array<EdgeID, 12> edgePositionD{
  EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB, EdgeID::DF, EdgeID::DL,
  EdgeID::DB, EdgeID::DR, EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};
static constexpr std::array<int, 12> edgeOrientationD{0, 0, 0, 0, 0, 0,
                                                    0, 0, 0, 0, 0, 0};

// Left move
static constexpr std::array<CornerID, 8> cornerPositionL{
  CornerID::URF, CornerID::ULB, CornerID::DBL, CornerID::UBR,
  CornerID::DFR, CornerID::UFL, CornerID::DLF, CornerID::DRB};
static constexpr std::array<int, 8> cornerOrientationL{0, 1, 2, 0,
                                                     0, 2, 1, 0};
static constexpr std::array<EdgeID, 12> edgePositionL{
  EdgeID::UR, EdgeID::UF, EdgeID::BL, EdgeID::UB, EdgeID::DR, EdgeID::DF,
  EdgeID::FL, EdgeID::DB, EdgeID::FR, EdgeID::UL, EdgeID::DL, EdgeID::BR};
static constexpr std::array<int, 12> edgeOrientationL{0, 0, 0, 0, 0, 0,
                                                    0, 0, 0, 0, 0, 0};

// Back move
static constexpr std::array<CornerID, 8> cornerPositionB{
  CornerID::URF, CornerID::UFL, CornerID::UBR, CornerID::DRB,
  CornerID::DFR, CornerID::DLF, CornerID::ULB, CornerID::DBL};
static constexpr std::array<int, 8> cornerOrientationB{0, 0, 1, 2,
                                                     0, 0, 2, 1};
static constexpr std::array<EdgeID, 12> edgePositionB{
  EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::BR, EdgeID::DR, EdgeID::DF,
  EdgeID::DL, EdgeID::BL, EdgeID::FR, EdgeID::FL, EdgeID::UB, EdgeID::DB};
static constexpr std::array<int, 12> edgeOrientationB{0, 0, 0, 1, 0, 0,
                                                    0, 1, 0, 0, 1, 1};

/*Coordinate mappings*/
static constexpr std::array<std::array<CubePosition, 3>, 8> cornerPositions = {
  CubePosition::U9, CubePosition::R1, CubePosition::F3,
  CubePosition::U7, CubePosition::F1, CubePosition::L3,
  CubePosition::U1, CubePosition::L1, CubePosition::B3,
  CubePosition::U3, CubePosition::B1, CubePosition::R3,
  CubePosition::D3, CubePosition::F9, CubePosition::R7,
  CubePosition::D1, CubePosition::L9, CubePosition::F7,
  CubePosition::D7, CubePosition::B9, CubePosition::L7,
  CubePosition::D9, CubePosition::R9, CubePosition::B7
};

static constexpr std::array<std::array<CubePosition, 2>, 12> edgePositions = {
  CubePosition::U6, CubePosition::R2,
  CubePosition::U8, CubePosition::F2,
  CubePosition::U4, CubePosition::L2,
  CubePosition::U2, CubePosition::B2,
  CubePosition::D6, CubePosition::R8,
  CubePosition::D2, CubePosition::F8,
  CubePosition::D4, CubePosition::L8,
  CubePosition::D8, CubePosition::B8,
  CubePosition::F6, CubePosition::R4,
  CubePosition::F4, CubePosition::L6,
  CubePosition::B6, CubePosition::L4,
  CubePosition::B4, CubePosition::R6
};

/*Color mappings*/
static constexpr std::array<std::array<ColorID, 3>, 12> cornerColorIDs = {
  ColorID::U, ColorID::R, ColorID::F,
  ColorID::U, ColorID::F, ColorID::L,
  ColorID::U, ColorID::L, ColorID::B,
  ColorID::U, ColorID::B, ColorID::R,
  ColorID::D, ColorID::F, ColorID::R,
  ColorID::D, ColorID::L, ColorID::F,
  ColorID::D, ColorID::B, ColorID::L,
  ColorID::D, ColorID::R, ColorID::B
};

static constexpr std::array<std::array<ColorID, 2>, 12> edgeColorIDs = {
  ColorID::U, ColorID::R,
  ColorID::U, ColorID::F,
  ColorID::U, ColorID::L,
  ColorID::U, ColorID::B,
  ColorID::D, ColorID::R,
  ColorID::D, ColorID::F,
  ColorID::D, ColorID::L,
  ColorID::D, ColorID::B,
  ColorID::F, ColorID::R,
  ColorID::F, ColorID::L,
  ColorID::B, ColorID::L,
  ColorID::B, ColorID::R
};

// clang-format on
};  // namespace Cube