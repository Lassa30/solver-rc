#include <cube/cube.hpp>
#include <kociemba-solver/solver.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>  // For potential debugging output
#include <string>
#include <vector>

using namespace Cube;

namespace Kociemba {

void KociembaSolver::generateFlipSliceTables() {
  const std::string fname1 = "fs_classidx";
  const std::string fname2 = "fs_sym";
  const std::string fname3 = "fs_rep";

  const std::string filepath1 = getPrecomputedPath(fname1);
  const std::string filepath2 = getPrecomputedPath(fname2);
  const std::string filepath3 = getPrecomputedPath(fname3);

  std::ifstream file_check1(filepath1, std::ios::binary);
  std::ifstream file_check2(filepath2, std::ios::binary);
  std::ifstream file_check3(filepath3, std::ios::binary);

  if (file_check1.good() && file_check2.good() && file_check3.good()) {
    file_check1.close();
    file_check2.close();
    file_check3.close();

    std::ifstream file1(filepath1, std::ios::binary);
    if (file1.is_open()) {
      flipSliceClassIdx_.resize(FLIP_MAX * SLICE_MAX);
      file1.read(reinterpret_cast<char*>(flipSliceClassIdx_.data()),
                 flipSliceClassIdx_.size() * sizeof(unsigned short));
      file1.close();
    } else {
      return;
    }

    std::ifstream file2(filepath2, std::ios::binary);
    if (file2.is_open()) {
      flipSliceSym_.resize(FLIP_MAX * SLICE_MAX);
      file2.read(reinterpret_cast<char*>(flipSliceSym_.data()), flipSliceSym_.size() * sizeof(unsigned char));
      file2.close();
    } else {
      return;
    }

    std::ifstream file3(filepath3, std::ios::binary);
    if (file3.is_open()) {
      flipSliceRep_.resize(FLIPSLICE_CLASS_MAX);
      file3.read(reinterpret_cast<char*>(flipSliceRep_.data()), flipSliceRep_.size() * sizeof(uint32_t));
      file3.close();
    } else {
      return;
    }
    return;
  }
  if (file_check1.is_open()) file_check1.close();
  if (file_check2.is_open()) file_check2.close();
  if (file_check3.is_open()) file_check3.close();

  flipSliceClassIdx_.assign(FLIP_MAX * SLICE_MAX, INVALID_SHORT);
  flipSliceSym_.assign(FLIP_MAX * SLICE_MAX, 0);
  flipSliceRep_.assign(FLIPSLICE_CLASS_MAX, 0);

  unsigned short classidx = 0;
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);

  for (int slc = 0; slc < SLICE_MAX; ++slc) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setSlice(slc);
    EdgeCornerCube cube_with_slice_set = solver_for_ops.getCube();

    for (int flip = 0; flip < FLIP_MAX; ++flip) {
      solver_for_ops.ecCube_ = cube_with_slice_set;
      solver_for_ops.coordCube.setFlip(flip);
      uint32_t idx = static_cast<uint32_t>(FLIP_MAX * slc + flip);

      if (flipSliceClassIdx_[idx] == INVALID_SHORT) {
        if (classidx >= FLIPSLICE_CLASS_MAX) {
          return;
        }
        flipSliceClassIdx_[idx] = classidx;
        flipSliceSym_[idx] = 0;
        flipSliceRep_[classidx] = idx;
      } else {
        continue;
      }

      EdgeCornerCube current_ec_state = solver_for_ops.getCube();
      for (int s = 0; s < N_SYM_D4h; ++s) {
        EdgeCornerCube ss = symCubes_[inverseSymIdx_[s]];
        ss.multiplyEdges(current_ec_state);
        ss.multiplyEdges(symCubes_[s]);

        KociembaSolver temp_solver_for_ss(ss);
        int new_slice = temp_solver_for_ss.coordCube.getSlice();
        int new_flip = temp_solver_for_ss.coordCube.getFlip();
        uint32_t idx_new = static_cast<uint32_t>(FLIP_MAX * new_slice + new_flip);

        if (flipSliceClassIdx_[idx_new] == INVALID_SHORT) {
          flipSliceClassIdx_[idx_new] = classidx;
          flipSliceSym_[idx_new] = static_cast<unsigned char>(s);
        }
      }
      classidx++;
    }
  }

  std::ofstream file1_out(filepath1, std::ios::binary);
  if (file1_out.is_open()) {
    file1_out.write(reinterpret_cast<const char*>(flipSliceClassIdx_.data()),
                    flipSliceClassIdx_.size() * sizeof(unsigned short));
    file1_out.close();
  }

  std::ofstream file2_out(filepath2, std::ios::binary);
  if (file2_out.is_open()) {
    file2_out.write(reinterpret_cast<const char*>(flipSliceSym_.data()), flipSliceSym_.size() * sizeof(unsigned char));
    file2_out.close();
  }

  std::ofstream file3_out(filepath3, std::ios::binary);
  if (file3_out.is_open()) {
    file3_out.write(reinterpret_cast<const char*>(flipSliceRep_.data()), flipSliceRep_.size() * sizeof(uint32_t));
    file3_out.close();
  }
}

void KociembaSolver::generateCornerTables() {
  const std::string fname1 = "co_classidx";
  const std::string fname2 = "co_sym";
  const std::string fname3 = "co_rep";

  const std::string filepath1 = getPrecomputedPath(fname1);
  const std::string filepath2 = getPrecomputedPath(fname2);
  const std::string filepath3 = getPrecomputedPath(fname3);

  std::ifstream file_check1(filepath1, std::ios::binary);
  std::ifstream file_check2(filepath2, std::ios::binary);
  std::ifstream file_check3(filepath3, std::ios::binary);

  if (file_check1.good() && file_check2.good() && file_check3.good()) {
    file_check1.close();
    file_check2.close();
    file_check3.close();

    std::ifstream file1(filepath1, std::ios::binary);
    if (file1.is_open()) {
      cornerClassIdx_.resize(CORNERS_MAX);
      file1.read(reinterpret_cast<char*>(cornerClassIdx_.data()), cornerClassIdx_.size() * sizeof(unsigned short));
      file1.close();
    } else {
      return;
    }

    std::ifstream file2(filepath2, std::ios::binary);
    if (file2.is_open()) {
      cornerSym_.resize(CORNERS_MAX);
      file2.read(reinterpret_cast<char*>(cornerSym_.data()), cornerSym_.size() * sizeof(unsigned char));
      file2.close();
    } else {
      return;
    }

    std::ifstream file3(filepath3, std::ios::binary);
    if (file3.is_open()) {
      cornerRep_.resize(CORNERS_CLASS_MAX);
      file3.read(reinterpret_cast<char*>(cornerRep_.data()), cornerRep_.size() * sizeof(unsigned short));
      file3.close();
    } else {
      return;
    }
    return;
  }
  if (file_check1.is_open()) file_check1.close();
  if (file_check2.is_open()) file_check2.close();
  if (file_check3.is_open()) file_check3.close();

  cornerClassIdx_.assign(CORNERS_MAX, INVALID_SHORT);
  cornerSym_.assign(CORNERS_MAX, 0);
  cornerRep_.assign(CORNERS_CLASS_MAX, 0);

  unsigned short classidx = 0;
  EdgeCornerCube base_cube_for_ops;
  KociembaSolver solver_for_ops(base_cube_for_ops);

  for (int cp = 0; cp < CORNERS_MAX; ++cp) {
    solver_for_ops.ecCube_ = EdgeCornerCube();
    solver_for_ops.coordCube.setCorners(cp);

    if (cornerClassIdx_[cp] == INVALID_SHORT) {
      if (classidx >= CORNERS_CLASS_MAX) {
        return;
      }
      cornerClassIdx_[cp] = classidx;
      cornerSym_[cp] = 0;
      cornerRep_[classidx] = static_cast<unsigned short>(cp);
    } else {
      continue;
    }

    EdgeCornerCube current_ec_state = solver_for_ops.getCube();
    for (int s = 0; s < N_SYM_D4h; ++s) {
      EdgeCornerCube ss = symCubes_[inverseSymIdx_[s]];
      ss.multiplyCorners(current_ec_state);
      ss.multiplyCorners(symCubes_[s]);

      KociembaSolver temp_solver_for_ss(ss);
      int cp_new = temp_solver_for_ss.coordCube.getCorners();

      if (cornerClassIdx_[cp_new] == INVALID_SHORT) {
        cornerClassIdx_[cp_new] = classidx;
        cornerSym_[cp_new] = static_cast<unsigned char>(s);
      }
    }
    classidx++;
  }

  std::ofstream file1_out(filepath1, std::ios::binary);
  if (file1_out.is_open()) {
    file1_out.write(reinterpret_cast<const char*>(cornerClassIdx_.data()),
                    cornerClassIdx_.size() * sizeof(unsigned short));
    file1_out.close();
  }

  std::ofstream file2_out(filepath2, std::ios::binary);
  if (file2_out.is_open()) {
    file2_out.write(reinterpret_cast<const char*>(cornerSym_.data()), cornerSym_.size() * sizeof(unsigned char));
    file2_out.close();
  }

  std::ofstream file3_out(filepath3, std::ios::binary);
  if (file3_out.is_open()) {
    file3_out.write(reinterpret_cast<const char*>(cornerRep_.data()), cornerRep_.size() * sizeof(unsigned short));
    file3_out.close();
  }
}

}  // namespace Kociemba
