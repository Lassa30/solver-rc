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

void KociembaSolver::generateTwistMoveTable() {
  const std::string filename = "move_twist";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      twistMoveTable_.resize(TWIST_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(twistMoveTable_.data()), twistMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  twistMoveTable_.assign(TWIST_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < TWIST_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setTwist(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyCorners(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        twistMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getTwist());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(twistMoveTable_.data()), twistMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateFlipMoveTable() {
  const std::string filename = "move_flip";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      flipMoveTable_.resize(FLIP_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(flipMoveTable_.data()), flipMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  flipMoveTable_.assign(FLIP_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < FLIP_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setFlip(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        flipMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getFlip());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(flipMoveTable_.data()), flipMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateSliceSortedMoveTable() {
  const std::string filename = "move_slice_sorted";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      sliceSortedMoveTable_.resize(SLICE_SORTED_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(sliceSortedMoveTable_.data()),
                sliceSortedMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  sliceSortedMoveTable_.assign(SLICE_SORTED_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < SLICE_SORTED_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setSliceSorted(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        sliceSortedMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getSliceSorted());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(sliceSortedMoveTable_.data()),
               sliceSortedMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateUEdgesMoveTable() {
  const std::string filename = "move_u_edges";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      uEdgesMoveTable_.resize(SLICE_SORTED_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(uEdgesMoveTable_.data()), uEdgesMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  uEdgesMoveTable_.assign(SLICE_SORTED_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < SLICE_SORTED_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setUEdges(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        uEdgesMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getUEdges());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(uEdgesMoveTable_.data()),
               uEdgesMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateDEdgesMoveTable() {
  const std::string filename = "move_d_edges";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      dEdgesMoveTable_.resize(SLICE_SORTED_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(dEdgesMoveTable_.data()), dEdgesMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  dEdgesMoveTable_.assign(SLICE_SORTED_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < SLICE_SORTED_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setDEdges(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        dEdgesMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getDEdges());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(dEdgesMoveTable_.data()),
               dEdgesMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateUDEdgesMoveTable() {
  const std::string filename = "move_ud_edges";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      udEdgesMoveTable_.resize(UD_EDGES_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(udEdgesMoveTable_.data()), udEdgesMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  udEdgesMoveTable_.assign(UD_EDGES_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < UD_EDGES_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setUDEdges(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyEdges(fundamentalMoveCubes[face_idx]);
        bool skip_store = false;
        if ((face_idx == 1 || face_idx == 2 || face_idx == 4 || face_idx == 5) && (power_idx != 1)) {
          skip_store = true;
        }

        if (!skip_store) {
          KociembaSolver temp_solver(working_cube);
          udEdgesMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
              static_cast<unsigned short>(temp_solver.coordCube.getUDEdges());
        }
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(udEdgesMoveTable_.data()),
               udEdgesMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateCornersMoveTable() {
  const std::string filename = "move_corners";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      cornersMoveTable_.resize(CORNERS_MAX * NUMBER_OF_MOVES);
      file.read(reinterpret_cast<char*>(cornersMoveTable_.data()), cornersMoveTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  cornersMoveTable_.assign(CORNERS_MAX * NUMBER_OF_MOVES, 0);
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);
  std::array<EdgeCornerCube, 6> fundamentalMoveCubes;
  for (int f = 0; f < 6; ++f) {
    fundamentalMoveCubes[f] = EdgeCornerCube::getMoveCube(static_cast<SideMovement>(f * 3));
  }

  for (int i = 0; i < CORNERS_MAX; ++i) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setCorners(i);
    EdgeCornerCube base_state_for_coord_i = solver_for_ops.getCube();

    for (int face_idx = 0; face_idx < 6; ++face_idx) {
      EdgeCornerCube working_cube = base_state_for_coord_i;
      for (int power_idx = 0; power_idx < 3; ++power_idx) {
        working_cube.multiplyCorners(fundamentalMoveCubes[face_idx]);
        KociembaSolver temp_solver(working_cube);
        cornersMoveTable_[NUMBER_OF_MOVES * i + face_idx * 3 + power_idx] =
            static_cast<unsigned short>(temp_solver.coordCube.getCorners());
      }
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(cornersMoveTable_.data()),
               cornersMoveTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

}  // namespace Kociemba
