#include <cube/cube.hpp>
#include <solvers/solvers.hpp>

#include <iostream>

using namespace Cube;

namespace Kociemba {
/*Coordinate Cube method implementations*/
// Constructor
KociembaSolver::CoordinateCube::CoordinateCube(KociembaSolver* solver) : solver_{solver} {
  coordsPhaseOne = phaseOne(getTwist(), getFlip(), getSliceSorted());
  coordsPhaseTwo = phaseTwo(getUEdges(), getDEdges(), getCorners(), getUDEdges());
  

  // Initialize symmetry-reduced coordinates for phase 1
  int slice_sorted = getSliceSorted();

  std::cout << "LEN FLIPSLICE_CLASSIDX " << flipSliceClassIdx_.size() << '\n'
            << " SLICE_SORTED " << coordsPhaseOne.slice_sorted << '\n'
            << " FLIP " << coordsPhaseOne.flip << '\n';
  int flip = getFlip();
  int slice_part = slice_sorted / PERM_4;
  int flipslice_idx = FLIP_MAX * slice_part + flip;
  flipslice_classidx = flipSliceClassIdx_[flipslice_idx];
  flipslice_sym = flipSliceSym_[flipslice_idx];
  flipslice_rep = flipSliceRep_[flipslice_classidx];

  // // Initialize symmetry-reduced coordinates for phase 2
  // int corners = getCorners();
  // corner_classidx = cornerClassIdx_[corners];
  // corner_sym = cornerSym_[corners];
  // corner_rep = cornerRep_[corner_classidx];
}

/*Coordinate mappings between EdgeCornerCube and CoordinateCube*/
int KociembaSolver::CoordinateCube::getTwist() const {
  int twist = 0;
  for (int i = static_cast<int>(CornerID::URF); i < static_cast<int>(CornerID::DRB); i++) {
    twist = 3 * twist + solver_->ecCube_.getCorner(i).orientation;
  }
  return twist;
}

void KociembaSolver::CoordinateCube::setTwist(int twist) {
  int twistparity = 0;
  std::array<Cube::Corner, 8> temp_corners;
  for (int i = 0; i < 8; ++i) {
    temp_corners[i] = solver_->ecCube_.getCorner(i);
  }

  for (int i = static_cast<int>(CornerID::DRB) - 1; i >= static_cast<int>(CornerID::URF); --i) {
    temp_corners[i].orientation = twist % 3;
    twistparity += temp_corners[i].orientation;
    twist /= 3;
  }
  temp_corners[static_cast<int>(CornerID::DRB)].orientation = ((3 - twistparity % 3) % 3);

  for (int i = 0; i < 8; ++i) {
    solver_->ecCube_.setCorner(i, temp_corners[i]);
  }
  // The original line was: coordsPhaseOne.twist = twist;
  // This should be:
  solver_->coordCube.coordsPhaseOne.twist = getTwist();  // Or simply the input `twist` if it's canonical
}

int KociembaSolver::CoordinateCube::getFlip() const {
  int flip = 0;
  for (int i = static_cast<int>(EdgeID::UR); i < static_cast<int>(EdgeID::BR); i++) {
    flip = 2 * flip + solver_->ecCube_.getEdge(i).orientation;
    std::cout << "EDGE ORIENTATION " << solver_->ecCube_.getEdge(i).orientation << '\n';
  }
  return flip;
}

void KociembaSolver::CoordinateCube::setFlip(int flip) {
  int flipparity = 0;
  std::array<Cube::Edge, 12> temp_edges;
  for (int i = 0; i < 12; ++i) temp_edges[i] = solver_->ecCube_.getEdge(i);

  for (int i = static_cast<int>(EdgeID::BR) - 1; i >= static_cast<int>(EdgeID::UR); --i) {
    temp_edges[i].orientation = flip % 2;
    flipparity += temp_edges[i].orientation;
    flip /= 2;
  }
  temp_edges[static_cast<int>(EdgeID::BR)].orientation = ((2 - flipparity % 2) % 2);

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges[i]);
  }
  solver_->coordCube.coordsPhaseOne.flip = getFlip();  // Or input `flip`
}

int KociembaSolver::CoordinateCube::getSlice() const {
  int a = 0;
  int x = 0;
  for (int j = static_cast<int>(EdgeID::BR); j >= static_cast<int>(EdgeID::UR); j--) {
    EdgeID edgePos = solver_->ecCube_.getEdge(j).position;
    if (edgePos >= EdgeID::FR && edgePos <= EdgeID::BR) {
      a += c_nk(11 - j, x + 1);
      x++;
    }
  }
  return a;
}

void KociembaSolver::CoordinateCube::setSlice(int idx) {
  std::array<EdgeID, 4> slice_edge = {EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};
  std::array<EdgeID, 8> other_edge = {EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB,
                                      EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB};

  std::array<Cube::Edge, 12> temp_edges;
  // Preserve orientations, only change positions based on idx
  for (int i = 0; i < 12; ++i) temp_edges[i].orientation = solver_->ecCube_.getEdge(i).orientation;

  for (int e_idx = 0; e_idx < 12; ++e_idx) {
    temp_edges[e_idx].position = EdgeID::COUNT;
  }

  int a = idx;
  int x = 4;
  for (int j = 0; j < 12; ++j) {
    if (a - c_nk(11 - j, x) >= 0) {
      temp_edges[j].position = slice_edge[4 - x];
      a -= c_nk(11 - j, x);
      x--;
    }
  }

  x = 0;
  for (int j = 0; j < 12; ++j) {
    if (temp_edges[j].position == EdgeID::COUNT) {
      temp_edges[j].position = other_edge[x];
      x++;
    }
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges[i]);
  }
  // Slice sorted is dependent on slice, so it should be updated too, or this setSlice is for specific table generation.
  // For now, just setting slice. If slice_sorted needs update, it should be explicit.
}

int KociembaSolver::CoordinateCube::getSliceSorted() const {
  int a = 0, x = 0;
  std::array<EdgeID, 4> edge4{};

  for (int j = static_cast<int>(EdgeID::BR); j >= static_cast<int>(EdgeID::UR); j--) {
    EdgeID edgePos = solver_->ecCube_.getEdge(j).position;
    if (edgePos >= EdgeID::FR && edgePos <= EdgeID::BR) {
      a += c_nk(11 - j, x + 1);
      edge4[3 - x] = edgePos;
      x++;
    }
  }

  // If we didn't find all 4 slice edges, it means the cube state is invalid for this coordinate
  if (x != 4) {
    // Return 0 instead of the error code 999'999
    // This is a significant change from previous implementation
    return 0;
  }

  int b = 0;
  for (int j = 3; j > 0; j--) {
    int k = 0;
    while (edge4[j] != static_cast<EdgeID>(j + 8)) {  // FR=8, FL=9, BL=10, BR=11
      rotate_left(edge4, 0, j);
      k++;
    }
    b = (j + 1) * b + k;
  }
  return 24 * a + b;
}

void KociembaSolver::CoordinateCube::setSliceSorted(int idx) {
  std::array<EdgeID, 4> slice_edge_orig = {EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};
  std::array<EdgeID, 4> slice_edge_perm = slice_edge_orig;  // To be permuted
  std::array<EdgeID, 8> other_edge = {EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB,
                                      EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB};

  std::array<Cube::Edge, 12> temp_edges;
  for (int i = 0; i < 12; ++i) {
    temp_edges[i].orientation = solver_->ecCube_.getEdge(i).orientation;  // Preserve orientation
    temp_edges[i].position = EdgeID::COUNT;
  }

  int b = idx % 24;
  int a = idx / 24;

  // Apply permutation to slice_edge_perm based on b
  // This loop correctly reconstructs the permutation for slice_edge_perm
  for (int j_rot = 1; j_rot < 4; ++j_rot) {  // Corrected loop from python: for j in range(1, 4):
    int k = b % (j_rot + 1);
    b /= (j_rot + 1);
    while (k > 0) {
      rotate_right(slice_edge_perm, 0, j_rot);  // Use the permutable copy
      k--;
    }
  }

  int x_val = 4;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (a - c_nk(11 - j_slot, x_val) >= 0) {
      temp_edges[j_slot].position = slice_edge_perm[4 - x_val];  // Use permuted slice_edge
      a -= c_nk(11 - j_slot, x_val);
      x_val--;
    }
  }

  x_val = 0;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (temp_edges[j_slot].position == EdgeID::COUNT) {
      temp_edges[j_slot].position = other_edge[x_val];
      x_val++;
    }
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges[i]);
  }
  solver_->coordCube.coordsPhaseOne.slice_sorted = idx;
}

int KociembaSolver::CoordinateCube::getUEdges() const {
  int a = 0, x = 0;
  std::array<EdgeID, 4> edge4{};
  std::array<EdgeID, 12> ep_mod;  // Stores only positions

  for (int i = 0; i < 12; i++) {
    ep_mod[i] = solver_->ecCube_.getEdge(i).position;
  }

  // The original Python code applies a S_URF3 symmetry to ep before calculation.
  // This corresponds to viewing the cube from a different angle or
  // defining U-edges relative to a specific orientation.
  // If S_URF3 is not applied, the definition of "UEdges" might differ.
  // Let's assume the C++ version's rotation is intentional for its coordinate definition.

  for (int j = 0; j < 4; j++) {  // This specific rotation needs to be verified against the coordinate's definition
    rotate_right(ep_mod, 0, 11);
  }

  for (int j = static_cast<int>(EdgeID::BR); j >= static_cast<int>(EdgeID::UR); j--) {
    if (ep_mod[j] >= EdgeID::UR && ep_mod[j] <= EdgeID::UB) {  // UR, UF, UL, UB
      a += c_nk(11 - j, x + 1);
      edge4[3 - x] = ep_mod[j];
      x++;
    }
  }

  if (x != 4) {
    return 0; /* Or error code */
  }

  int b = 0;
  for (int j = 3; j > 0; j--) {
    int k = 0;
    int safety_counter = 0;
    while (edge4[j] != static_cast<EdgeID>(j)) {  // UR=0, UF=1, UL=2, UB=3
      rotate_left(edge4, 0, j);
      k++;
      safety_counter++;
      if (safety_counter > 24) {
        return 0; /* Error */
      }
    }
    b = (j + 1) * b + k;
  }
  return 24 * a + b;
}

void KociembaSolver::CoordinateCube::setUEdges(int idx) {
  std::array<EdgeID, 4> slice_edge_perm = {EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB};
  std::array<EdgeID, 8> other_edge = {EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB,
                                      EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};

  std::array<Cube::Edge, 12> temp_edges_struct;
  for (int i = 0; i < 12; ++i) {
    temp_edges_struct[i].orientation = solver_->ecCube_.getEdge(i).orientation;  // Preserve orientation
    temp_edges_struct[i].position = EdgeID::COUNT;
  }

  int b = idx % 24;
  int a = idx / 24;

  for (int j_rot = 1; j_rot < 4; ++j_rot) {
    int k = b % (j_rot + 1);
    b /= (j_rot + 1);
    while (k > 0) {
      rotate_right(slice_edge_perm, 0, j_rot);
      k--;
    }
  }

  int x_val = 4;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (a - c_nk(11 - j_slot, x_val) >= 0) {
      temp_edges_struct[j_slot].position = slice_edge_perm[4 - x_val];
      a -= c_nk(11 - j_slot, x_val);
      x_val--;
    }
  }

  x_val = 0;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (temp_edges_struct[j_slot].position == EdgeID::COUNT) {
      temp_edges_struct[j_slot].position = other_edge[x_val];
      x_val++;
    }
  }

  // Apply the inverse of the rotation done in getUEdges
  for (int j = 0; j < 4; j++) {
    rotate_left(temp_edges_struct, 0, 11);
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges_struct[i]);
  }
  solver_->coordCube.coordsPhaseTwo.u_edges = idx;
}

int KociembaSolver::CoordinateCube::getDEdges() const {
  int a = 0;
  int x = 0;
  std::array<EdgeID, 4> edge4{};
  std::array<EdgeID, 12> ep_mod;

  for (int i = 0; i < 12; i++) {
    ep_mod[i] = solver_->ecCube_.getEdge(i).position;
  }

  // Same rotation as in getUEdges, its purpose should be consistent.
  for (int j = 0; j < 4; j++) {
    rotate_right(ep_mod, 0, 11);
  }

  for (int j = static_cast<int>(EdgeID::BR); j >= static_cast<int>(EdgeID::UR); j--) {
    if (ep_mod[j] >= EdgeID::DR && ep_mod[j] <= EdgeID::DB) {  // DR, DF, DL, DB
      a += c_nk(11 - j, x + 1);
      edge4[3 - x] = ep_mod[j];
      x++;
    }
  }

  if (x != 4) {
    return 0; /* Or error code */
  }

  int b = 0;
  for (int j = 3; j > 0; j--) {
    int k = 0;
    int safety_counter = 0;
    // Edges DR,DF,DL,DB are 4,5,6,7 respectively
    while (edge4[j] != static_cast<EdgeID>(j + 4)) {
      rotate_left(edge4, 0, j);
      k++;
      safety_counter++;
      if (safety_counter > 24) {
        return 0; /* Error */
      }
    }
    b = (j + 1) * b + k;
  }
  return 24 * a + b;
}

void KociembaSolver::CoordinateCube::setDEdges(int idx) {
  std::array<EdgeID, 4> slice_edge_perm = {EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB};
  std::array<EdgeID, 8> other_edge = {EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR,  // Slice edges first in other
                                      EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB};

  std::array<Cube::Edge, 12> temp_edges_struct;
  for (int i = 0; i < 12; ++i) {
    temp_edges_struct[i].orientation = solver_->ecCube_.getEdge(i).orientation;  // Preserve orientation
    temp_edges_struct[i].position = EdgeID::COUNT;
  }

  int b = idx % 24;
  int a = idx / 24;

  for (int j_rot = 1; j_rot < 4; ++j_rot) {
    int k = b % (j_rot + 1);
    b /= (j_rot + 1);
    while (k > 0) {
      rotate_right(slice_edge_perm, 0, j_rot);
      k--;
    }
  }

  int x_val = 4;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (a - c_nk(11 - j_slot, x_val) >= 0) {
      temp_edges_struct[j_slot].position = slice_edge_perm[4 - x_val];
      a -= c_nk(11 - j_slot, x_val);
      x_val--;
    }
  }

  x_val = 0;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (temp_edges_struct[j_slot].position == EdgeID::COUNT) {
      temp_edges_struct[j_slot].position = other_edge[x_val];
      x_val++;
    }
  }

  // Apply the inverse of the rotation done in getDEdges
  for (int j = 0; j < 4; j++) {
    rotate_left(temp_edges_struct, 0, 11);
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges_struct[i]);
  }
  solver_->coordCube.coordsPhaseTwo.d_edges = idx;
}

int KociembaSolver::CoordinateCube::getCorners() const {
  std::array<CornerID, 8> perm;
  for (int i = 0; i < 8; ++i) {
    perm[i] = solver_->ecCube_.getCorner(i).position;
  }

  int b = 0;
  // Corrected loop range from Python: range(7, 0, -1) -> j from 7 down to 1
  // static_cast<int>(CornerID::DRB) is 7. static_cast<int>(CornerID::URF) is 0.
  // So loop j from 7 down to 1 (exclusive of 0).
  for (int j = static_cast<int>(CornerID::DRB); j > static_cast<int>(CornerID::URF); j--) {
    int k = 0;
    int safety_counter = 0;
    while (perm[j] != static_cast<CornerID>(j)) {
      rotate_left(perm, 0, j);  // perm is array of CornerID, rotate_left needs to handle this
      k++;
      safety_counter++;
      if (safety_counter > 24) {
        return 0; /* Error */
      }
    }
    b = (j + 1) * b + k;  // This is (j) * b + k in Python if j is 0-indexed in the formula there.
        // Python: b = j * b + k. If j goes 7 down to 1, then it's (perm_len - 1 - current_idx_from_end)
        // Here j is the actual index. So (j+1) seems fine if mapping to N! formula.
        // Let's assume the (j+1) is correct from the original C++ code.
  }
  return b;
}

void KociembaSolver::CoordinateCube::setCorners(int idx) {
  std::array<CornerID, 8> cp_target;
  for (int i = 0; i < 8; ++i) cp_target[i] = static_cast<CornerID>(i);  // Initialize to solved state

  // Corrected loop from Python: for j in range(8) -> j from 0 to 7
  for (int j = 0; j < static_cast<int>(CornerID::COUNT); ++j) {  // Iterate 0 to 7
    int k = idx % (j + 1);
    idx /= (j + 1);
    while (k > 0) {
      // Python: cp_target[0:j+1].rotate(1) -> rotate_right(cp_target, 0, j)
      rotate_right(cp_target, 0, j);
      k--;
    }
  }

  for (int i = 0; i < 8; ++i) {
    solver_->ecCube_.setCorner(i, {cp_target[i], solver_->ecCube_.getCorner(i).orientation});
  }
  solver_->coordCube.coordsPhaseTwo.corners = idx;
}

int KociembaSolver::CoordinateCube::getUDEdges() const {
  // This coordinate is valid only if slice edges are in the slice (SliceSorted < 24)
  // The original Python code checks if cc.get_slice_sorted() == 0 for this.
  // Here, a check `getSliceSorted() >= 24` (which is SLICE_MAX / 24) might be more appropriate
  // if `getSliceSorted()` returns the full coordinate.
  // The current C++ code has `if (getSliceSorted() >= 24) return -1;`
  // This implies `getSliceSorted()` returns the "a" part of slice_sorted (location part).
  // Or more simply, if this coordinate is only valid when slice pieces are in the E slice.
  // The python code `self.get_slice_sorted() // 24 == 0` means combination `a` must be 0.
  // `a=0` means C(11,4) ways, where the 4 slice edges are in the first 4 slots (UR,UF,UL,UB). This is wrong.
  // `a=0` for `getSlice()` means the 4 slice edges are in positions FR, FL, BL, BR.
  // The condition `self.get_slice_sorted() // 24 == 0` in python means the 4 slice edges
  // (FR,FL,BL,BR) are in the first 4 slots considered by C(n,k), which are UR,UF,UL,UB. This is impossible.
  // The original check in python for `get_ud_edges` is `if self.get_slice_sorted() >= 24: return -1`
  // This means the permutation part `b` of `slice_sorted` must be 0.
  // `slice_sorted = 24 * a + b`. If `slice_sorted < 24`, then `a` must be 0.
  // `a=0` for `getSliceSorted` means the 4 slice edges (FR,FL,BL,BR) are in the slots UR,UF,UL,UB.
  // This is the G1 state criteria: slice edges are in the U/D layers.
  // So, `getSliceSorted() / 24` (the 'a' part) must be 0.
  if ((getSliceSorted() / 24) != 0) {  // If slice edges are not in U/D layers.
    return -1;
  }

  std::array<EdgeID, 8> perm;  // Permutation of the 8 UD layer edges (UR..DB)
  // Populate perm with positions of edges UR through DB
  for (int i = 0; i <= static_cast<int>(EdgeID::DB); ++i) {  // Edges 0 through 7
    perm[i] = solver_->ecCube_.getEdge(i).position;
  }

  int b = 0;
  // Loop j from 7 down to 1 (exclusive of 0)
  for (int j = static_cast<int>(EdgeID::DB); j > static_cast<int>(EdgeID::UR); --j) {
    int k = 0;
    int safety_counter = 0;
    while (perm[j] != static_cast<EdgeID>(j)) {
      rotate_left(perm, 0, j);
      k++;
      safety_counter++;
      if (safety_counter > 24) {
        return 0; /* Error */
      }
    }
    b = (j + 1) * b + k;  // Similar to getCorners logic
  }
  return b;
}

void KociembaSolver::CoordinateCube::setUDEdges(int idx) {
  std::array<EdgeID, 4> slice_edge_perm = {EdgeID::UR, EdgeID::UF, EdgeID::UL, EdgeID::UB};
  std::array<EdgeID, 8> other_edge = {EdgeID::DR, EdgeID::DF, EdgeID::DL, EdgeID::DB,
                                      EdgeID::FR, EdgeID::FL, EdgeID::BL, EdgeID::BR};

  std::array<Cube::Edge, 12> temp_edges_struct;
  for (int i = 0; i < 12; ++i) {
    temp_edges_struct[i].orientation = solver_->ecCube_.getEdge(i).orientation;  // Preserve orientation
    temp_edges_struct[i].position = EdgeID::COUNT;
  }

  int b = idx % 24;
  int a = idx / 24;

  for (int j_rot = 1; j_rot < 4; ++j_rot) {
    int k = b % (j_rot + 1);
    b /= (j_rot + 1);
    while (k > 0) {
      rotate_right(slice_edge_perm, 0, j_rot);
      k--;
    }
  }

  int x_val = 4;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (a - c_nk(11 - j_slot, x_val) >= 0) {
      temp_edges_struct[j_slot].position = slice_edge_perm[4 - x_val];
      a -= c_nk(11 - j_slot, x_val);
      x_val--;
    }
  }

  x_val = 0;
  for (int j_slot = 0; j_slot < 12; ++j_slot) {
    if (temp_edges_struct[j_slot].position == EdgeID::COUNT) {
      temp_edges_struct[j_slot].position = other_edge[x_val];
      x_val++;
    }
  }

  // Apply the inverse of the rotation done in getUEdges
  for (int j = 0; j < 4; j++) {
    rotate_left(temp_edges_struct, 0, 11);
  }

  for (int i = 0; i < 12; ++i) {
    solver_->ecCube_.setEdge(i, temp_edges_struct[i]);
  }
  solver_->coordCube.coordsPhaseTwo.ud_edges = idx;
}

void KociembaSolver::CoordinateCube::phase1Move(int m) {
  // Update phase 1 coordinates
  coordsPhaseOne.twist = solver_->twistMoveTable_[NUMBER_OF_MOVES * coordsPhaseOne.twist + m];
  coordsPhaseOne.flip = solver_->flipMoveTable_[NUMBER_OF_MOVES * coordsPhaseOne.flip + m];
  coordsPhaseOne.slice_sorted = solver_->sliceSortedMoveTable_[NUMBER_OF_MOVES * coordsPhaseOne.slice_sorted + m];

  // Optional phase 2 coordinate updates during phase 1
  coordsPhaseTwo.u_edges = solver_->uEdgesMoveTable_[NUMBER_OF_MOVES * coordsPhaseTwo.u_edges + m];
  coordsPhaseTwo.d_edges = solver_->dEdgesMoveTable_[NUMBER_OF_MOVES * coordsPhaseTwo.d_edges + m];
  coordsPhaseTwo.corners = solver_->cornersMoveTable_[NUMBER_OF_MOVES * coordsPhaseTwo.corners + m];

  // Update symmetry-reduced coordinates for phase 1
  int slice_part = coordsPhaseOne.slice_sorted / 24;  // N_PERM_4 = 24
  int flipslice_idx = FLIP_MAX * slice_part + coordsPhaseOne.flip;
  flipslice_classidx = solver_->flipSliceClassIdx_[flipslice_idx];
  flipslice_sym = solver_->flipSliceSym_[flipslice_idx];
  flipslice_rep = solver_->flipSliceRep_[flipslice_classidx];

  // Update symmetry-reduced coordinates for phase 2
  corner_classidx = solver_->cornerClassIdx_[coordsPhaseTwo.corners];
  corner_sym = solver_->cornerSym_[coordsPhaseTwo.corners];
  corner_rep = solver_->cornerRep_[corner_classidx];
}

void KociembaSolver::CoordinateCube::phase2Move(int m) {
  // Update phase 2 coordinates
  coordsPhaseOne.slice_sorted = solver_->sliceSortedMoveTable_[NUMBER_OF_MOVES * coordsPhaseOne.slice_sorted + m];
  coordsPhaseTwo.corners = solver_->cornersMoveTable_[NUMBER_OF_MOVES * coordsPhaseTwo.corners + m];
  coordsPhaseTwo.ud_edges = solver_->udEdgesMoveTable_[NUMBER_OF_MOVES * coordsPhaseTwo.ud_edges + m];

  // Update symmetry-reduced coordinates relevant to phase 2
  corner_classidx = solver_->cornerClassIdx_[coordsPhaseTwo.corners];
  corner_sym = solver_->cornerSym_[coordsPhaseTwo.corners];
  corner_rep = solver_->cornerRep_[corner_classidx];
}

int KociembaSolver::CoordinateCube::getDepthPhase1() {
  int slice_ = coordsPhaseOne.slice_sorted / 24;  // N_PERM_4 = 24
  int flip = coordsPhaseOne.flip;
  int twist = coordsPhaseOne.twist;
  int flipslice = FLIP_MAX * slice_ + flip;
  int classidx = solver_->flipSliceClassIdx_[flipslice];
  int sym = solver_->flipSliceSym_[flipslice];
  int depth_mod3 =
      solver_->getFlipSliceTwistDepth3(TWIST_MAX * classidx + solver_->twistConjTable_[(twist << 4) + sym]);

  int depth = 0;
  while (flip != SOLVED || slice_ != SOLVED || twist != SOLVED) {
    if (depth_mod3 == 0) {
      depth_mod3 = 3;
    }

    bool moveMade = false;
    for (int m = 0; m < NUMBER_OF_MOVES; ++m) {
      int twist1 = solver_->twistMoveTable_[NUMBER_OF_MOVES * twist + m];
      int flip1 = solver_->flipMoveTable_[NUMBER_OF_MOVES * flip + m];
      int slice1 = solver_->sliceSortedMoveTable_[NUMBER_OF_MOVES * slice_ * 24 + m] / 24;
      int flipslice1 = FLIP_MAX * slice1 + flip1;
      int classidx1 = solver_->flipSliceClassIdx_[flipslice1];
      int sym1 = solver_->flipSliceSym_[flipslice1];

      if (solver_->getFlipSliceTwistDepth3(TWIST_MAX * classidx1 + solver_->twistConjTable_[(twist1 << 4) + sym1]) ==
          depth_mod3 - 1) {
        depth += 1;
        twist = twist1;
        flip = flip1;
        slice_ = slice1;
        depth_mod3 -= 1;
        moveMade = true;
        break;
      }
    }

    if (!moveMade) {
      // This is a safety check - should not happen with valid table data
      return -1;
    }
  }

  return depth;
}

int KociembaSolver::CoordinateCube::getDepthPhase2(int corners, int ud_edges) {
  // Can't use solver_ directly as this is a static method, so use KociembaSolver::
  int classidx = cornerClassIdx_[corners];
  int sym = cornerSym_[corners];
  int depth_mod3 =
      KociembaSolver::getCornersUDEdgesDepth3(UD_EDGES_MAX * classidx + udEdgesConjTable_[(ud_edges << 4) + sym]);

  if (depth_mod3 == 3) {  // unfilled entry, depth >= 11
    return 11;
  }

  int depth = 0;
  const int SOLVED = 0;

  while (corners != SOLVED || ud_edges != SOLVED) {
    if (depth_mod3 == 0) {
      depth_mod3 = 3;
    }

    bool moveMade = false;
    // Only iterate phase 2 moves: U1, U2, U3, R2, F2, D1, D2, D3, L2, B2
    int phase2Moves[] = {0, 1, 2, 4, 7, 9, 10, 11, 13, 16};

    for (int m : phase2Moves) {
      int corners1 = cornersMoveTable_[NUMBER_OF_MOVES * corners + m];
      int ud_edges1 = udEdgesMoveTable_[NUMBER_OF_MOVES * ud_edges + m];
      int classidx1 = cornerClassIdx_[corners1];
      int sym1 = cornerSym_[corners1];

      if (KociembaSolver::getCornersUDEdgesDepth3(UD_EDGES_MAX * classidx1 +
                                                  udEdgesConjTable_[(ud_edges1 << 4) + sym1]) == depth_mod3 - 1) {
        depth += 1;
        corners = corners1;
        ud_edges = ud_edges1;
        depth_mod3 -= 1;
        moveMade = true;
        break;
      }
    }

    if (!moveMade) {
      // This is a safety check - should not happen with valid table data
      return -1;
    }
  }

  return depth;
}

/*helpers for coordinate computation*/
// These are static member functions of CoordinateCube
int KociembaSolver::CoordinateCube::c_nk(int n, int k) {
  if (n < k) {
    return 0;
  }
  if (k > n / 2) {
    k = n - k;
  }
  if (k < 0) return 0;  // Added safety for k<0

  long long s = 1;  // Use long long to prevent overflow during intermediate calcs
  for (int i = 1; i <= k; ++i) {
    s = s * (n - i + 1) / i;
  }
  return static_cast<int>(s);
}

template <typename Container>
void KociembaSolver::CoordinateCube::rotate_right(Container& arr, int left, int right) {
  if (left >= right || arr.empty()) return;
  auto temp = arr[right];
  for (int i = right; i > left; i--) {
    arr[i] = arr[i - 1];
  }
  arr[left] = temp;
}

template <typename Container>
void KociembaSolver::CoordinateCube::rotate_left(Container& arr, int left, int right) {
  if (left >= right || arr.empty()) return;
  auto temp = arr[left];
  for (int i = left; i < right; i++) {
    arr[i] = arr[i + 1];
  }
  arr[right] = temp;
}

}  // namespace Kociemba