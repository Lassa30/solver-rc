#include <cube/cube.hpp>
#include <kociemba-solver/solver.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>  // For potential debugging output
#include <string>
#include <vector>

using namespace Cube;

namespace Kociemba {

void KociembaSolver::generateTwistConjTable() {
  const std::string filename = "conj_twist";
  const std::string filepath = getPrecomputedPath(filename);

  // Check if file already exists
  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    // File exists, load the table
    file_check.close();

    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      twistConjTable_.resize(TWIST_MAX * N_SYM_D4h);
      file.read(reinterpret_cast<char*>(twistConjTable_.data()), twistConjTable_.size() * sizeof(unsigned short));
      file.close();
    }
    return;
  }

  // File doesn't exist, create the table
  twistConjTable_.resize(TWIST_MAX * N_SYM_D4h, 0);

  EdgeCornerCube base_cube_for_twist_setting;
  KociembaSolver solver_for_twist_setting(base_cube_for_twist_setting);

  for (int t = 0; t < TWIST_MAX; t++) {
    solver_for_twist_setting.ecCube_ = EdgeCornerCube();
    solver_for_twist_setting.coordCube.setTwist(t);
    EdgeCornerCube cc_with_twist = solver_for_twist_setting.getCube();

    for (int s = 0; s < N_SYM_D4h; s++) {
      EdgeCornerCube ss = symCubes_[s];
      ss.multiplyCorners(cc_with_twist);
      ss.multiplyCorners(symCubes_[inverseSymIdx_[s]]);
      KociembaSolver temp_solver(ss);
      int resulting_twist = temp_solver.coordCube.getTwist();
      twistConjTable_[N_SYM_D4h * t + s] = static_cast<unsigned short>(resulting_twist);
    }
  }

  std::ofstream file(filepath, std::ios::binary);
  if (file.is_open()) {
    file.write(reinterpret_cast<const char*>(twistConjTable_.data()), twistConjTable_.size() * sizeof(unsigned short));
    file.close();
  }
}

void KociembaSolver::generateUDEdgesConjTable() {
  const std::string filename = "conj_ud_edges";
  const std::string filepath = getPrecomputedPath(filename);

  std::ifstream file_check(filepath, std::ios::binary);
  if (file_check.good()) {
    file_check.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      udEdgesConjTable_.resize(UD_EDGES_MAX * N_SYM_D4h);
      file.read(reinterpret_cast<char*>(udEdgesConjTable_.data()), udEdgesConjTable_.size() * sizeof(unsigned short));
      file.close();
    } else {
      goto generate_table;
    }
    return;
  }

generate_table:
  udEdgesConjTable_.resize(UD_EDGES_MAX * N_SYM_D4h);

  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);

  for (int t = 0; t < UD_EDGES_MAX; ++t) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setUDEdges(t);
    EdgeCornerCube cc_t_state = solver_for_ops.getCube();

    for (int s = 0; s < N_SYM_D4h; ++s) {
      EdgeCornerCube ss = symCubes_[s];                // Copy symmetry cube S_s
      ss.multiplyEdges(cc_t_state);                    // ss = S_s * C_t
      ss.multiplyEdges(symCubes_[inverseSymIdx_[s]]);  // ss = (S_s * C_t) * S_s^-1

      KociembaSolver temp_solver_result(ss);
      udEdgesConjTable_[N_SYM_D4h * t + s] = static_cast<unsigned short>(temp_solver_result.coordCube.getUDEdges());
    }
  }

  std::ofstream outFile(filepath, std::ios::binary);
  if (outFile.is_open()) {
    outFile.write(reinterpret_cast<const char*>(udEdgesConjTable_.data()),
                  udEdgesConjTable_.size() * sizeof(unsigned short));
    outFile.close();
  }
}

}  // namespace Kociemba
