#include <cube/cube.hpp>
#include <solvers/solvers.hpp>

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>  // For potential debugging output
#include <string>
#include <vector>

using namespace Cube;

namespace Kociemba {

/*static arrays for basic symmetry operations - file static*/
static const std::array<CornerID, 8> cpROT_URF3 = {CornerID::URF, CornerID::DFR, CornerID::DLF, CornerID::UFL,
                                                   CornerID::UBR, CornerID::DRB, CornerID::DBL, CornerID::ULB};
static const std::array<int, 8> coROT_URF3 = {1, 2, 1, 2, 2, 1, 2, 1};
static const std::array<EdgeID, 12> epROT_URF3 = {EdgeID::UF, EdgeID::FR, EdgeID::DF, EdgeID::FL,
                                                  EdgeID::UB, EdgeID::BR, EdgeID::DB, EdgeID::BL,
                                                  EdgeID::UR, EdgeID::DR, EdgeID::DL, EdgeID::UL};
static const std::array<int, 12> eoROT_URF3 = {1, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1};

static const std::array<CornerID, 8> cpROT_F2 = {CornerID::DLF, CornerID::DFR, CornerID::DRB, CornerID::DBL,
                                                 CornerID::UFL, CornerID::URF, CornerID::UBR, CornerID::ULB};
static const std::array<int, 8> coROT_F2 = {0, 0, 0, 0, 0, 0, 0, 0};
static const std::array<EdgeID, 12> epROT_F2 = {EdgeID::DL, EdgeID::DF, EdgeID::DR, EdgeID::DB, EdgeID::UL, EdgeID::UF,
                                                EdgeID::UR, EdgeID::UB, EdgeID::FL, EdgeID::FR, EdgeID::BR, EdgeID::BL};
static const std::array<int, 12> eoROT_F2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

static const std::array<CornerID, 8> cpROT_U4 = {CornerID::UBR, CornerID::URF, CornerID::UFL, CornerID::ULB,
                                                 CornerID::DRB, CornerID::DFR, CornerID::DLF, CornerID::DBL};
static const std::array<int, 8> coROT_U4 = {0, 0, 0, 0, 0, 0, 0, 0};
static const std::array<EdgeID, 12> epROT_U4 = {EdgeID::UB, EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::DB, EdgeID::DR,
                                                EdgeID::DF, EdgeID::DL, EdgeID::BR, EdgeID::FR, EdgeID::FL, EdgeID::BL};
static const std::array<int, 12> eoROT_U4 = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1};

static const std::array<CornerID, 8> cpMIRR_LR2 = {CornerID::UFL, CornerID::URF, CornerID::UBR, CornerID::ULB,
                                                   CornerID::DLF, CornerID::DFR, CornerID::DRB, CornerID::DBL};
static const std::array<int, 8> coMIRR_LR2 = {3, 3, 3, 3, 3, 3, 3, 3};
static const std::array<EdgeID, 12> epMIRR_LR2 = {EdgeID::UL, EdgeID::UF, EdgeID::UR, EdgeID::UB,
                                                  EdgeID::DL, EdgeID::DF, EdgeID::DR, EdgeID::DB,
                                                  EdgeID::FL, EdgeID::FR, EdgeID::BR, EdgeID::BL};
static const std::array<int, 12> eoMIRR_LR2 = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void KociembaSolver::generateBasicSymCubes() {
  basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_URF3)] =
      EdgeCornerCube(epROT_URF3, eoROT_URF3, cpROT_URF3, coROT_URF3);

  basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_F2)] = EdgeCornerCube(epROT_F2, eoROT_F2, cpROT_F2, coROT_F2);

  basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_U4)] = EdgeCornerCube(epROT_U4, eoROT_U4, cpROT_U4, coROT_U4);

  basicSymCubes_[static_cast<int>(BasicSymmetry::MIRR_LR2)] =
      EdgeCornerCube(epMIRR_LR2, eoMIRR_LR2, cpMIRR_LR2, coMIRR_LR2);
}

void KociembaSolver::generateSymCubes() {
  EdgeCornerCube cc;
  int idx = 0;

  for (int urf3 = 0; urf3 < 3; urf3++) {
    for (int f2 = 0; f2 < 2; f2++) {
      for (int u4 = 0; u4 < 4; u4++) {
        for (int lr2 = 0; lr2 < 2; lr2++) {
          symCubes_[idx] = cc;
          idx++;
          cc.multiply(basicSymCubes_[static_cast<int>(BasicSymmetry::MIRR_LR2)]);
        }
        cc.multiply(basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_U4)]);
      }
      cc.multiply(basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_F2)]);
    }
    cc.multiply(basicSymCubes_[static_cast<int>(BasicSymmetry::ROT_URF3)]);
  }
}

void KociembaSolver::generateInverseSymIdx() {
  for (int j = 0; j < NUMBER_OF_SYMMETRIES; j++) {
    for (int i = 0; i < NUMBER_OF_SYMMETRIES; i++) {
      EdgeCornerCube tempCube = symCubes_[j];
      tempCube.multiply(symCubes_[i]);

      if (tempCube == EdgeCornerCube()) {
        inverseSymIdx_[j] = i;
        break;
      }
    }
  }
}

void KociembaSolver::generateConjMove() {
  // File doesn't exist or couldn't be read, create the table
  for (int i = 0; i < NUMBER_OF_MOVES * NUMBER_OF_SYMMETRIES; i++) {
    conjMoves_[i] = 0;
  }

  // For each symmetry
  for (int s = 0; s < NUMBER_OF_SYMMETRIES; s++) {
    // For each possible move
    for (int m = 0; m < NUMBER_OF_MOVES; m++) {
      // Create a cube with the symmetry transformation
      EdgeCornerCube ss = symCubes_[s];

      // Apply move m to it (s*m)
      ss.multiply(EdgeCornerCube::getMoveCube(static_cast<SideMovement>(m)));

      // Apply inverse symmetry (s*m*s^-1)
      ss.multiply(symCubes_[inverseSymIdx_[s]]);

      // Find which move this is equivalent to
      for (int m2 = 0; m2 < NUMBER_OF_MOVES; m2++) {
        if (ss == EdgeCornerCube::getMoveCube(static_cast<SideMovement>(m2))) {
          conjMoves_[NUMBER_OF_MOVES * s + m] = m2;
          break;
        }
      }
    }
  }
}

}  // namespace Kociemba
