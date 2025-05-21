#include <cube/cube.hpp>
#include <solvers/solvers.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>  // For potential debugging output
#include <string>
#include <vector>

using namespace Cube;

namespace Kociemba {

void KociembaSolver::generateCornersUDEdgesDepth3Table() {
  const int total = CORNERS_CLASS_MAX * UD_EDGES_MAX;
  const std::string fname = "phase2_prun";
  const std::string filepath = getPrecomputedPath(fname);

  std::ifstream file_in(filepath, std::ios::binary);
  if (file_in.good()) {
    file_in.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      cornersUDEdgesDepth3_.resize(total / 16);
      file.read(reinterpret_cast<char*>(cornersUDEdgesDepth3_.data()), cornersUDEdgesDepth3_.size() * sizeof(uint32_t));
      file.close();
    }
    return;
  }

  // Table doesn't exist, create it
  cornersUDEdgesDepth3_.assign(total / 16, 0xffffffffu);

  // Create table with the symmetries of the corners classes
  std::vector<unsigned short> c_sym(CORNERS_CLASS_MAX, 0);
  for (int i = 0; i < CORNERS_CLASS_MAX; ++i) {
    int rep = cornerRep_[i];
    EdgeCornerCube cc;
    KociembaSolver solver(cc);
    solver.coordCube.setCorners(rep);
    cc = solver.getCube();
    for (int s = 0; s < N_SYM_D4h; ++s) {
      EdgeCornerCube ss = symCubes_[s];
      ss.multiplyCorners(cc);
      ss.multiplyCorners(symCubes_[inverseSymIdx_[s]]);
      KociembaSolver temp_solver(ss);
      if (temp_solver.coordCube.getCorners() == rep) {
        c_sym[i] |= (1 << s);
      }
    }
  }

  // Initialize the search
  int c_classidx = 0;  // value for solved phase 2
  int ud_edge = 0;
  setCornersUDEdgesDepth3(UD_EDGES_MAX * c_classidx + ud_edge, 0);
  int done = 1;
  int depth = 0;

  while (depth < 10) {  // Fill the table only to depth 9 + 1
    int depth3 = depth % 3;
    int idx = 0;
    int mult = (depth > 9) ? 1 : 2;

    for (c_classidx = 0; c_classidx < CORNERS_CLASS_MAX; ++c_classidx) {
      ud_edge = 0;
      while (ud_edge < UD_EDGES_MAX) {
        // Fast skip for empty regions
        if (idx % 16 == 0 && cornersUDEdgesDepth3_[idx / 16] == 0xffffffffu && ud_edge < UD_EDGES_MAX - 16) {
          ud_edge += 16;
          idx += 16;
          continue;
        }

        if (getCornersUDEdgesDepth3(idx) == depth3) {
          int corner = cornerRep_[c_classidx];
          // Only phase 2 moves: U1, U2, U3, R2, F2, D1, D2, D3, L2, B2
          for (int m : {0, 1, 2,   /*U1,U2,U3*/
                        4,         /*R2*/
                        7,         /*F2*/
                        9, 10, 11, /*D1,D2,D3*/
                        13,        /*L2*/
                        16 /*B2*/}) {
            int ud_edge1 = udEdgesMoveTable_[NUMBER_OF_MOVES * ud_edge + m];
            int corner1 = cornersMoveTable_[NUMBER_OF_MOVES * corner + m];
            int c1_classidx = cornerClassIdx_[corner1];
            int c1_sym = cornerSym_[corner1];
            ud_edge1 = udEdgesConjTable_[(ud_edge1 << 4) + c1_sym];  // N_SYM_D4h is 16, so << 4 is correct
            int idx1 = UD_EDGES_MAX * c1_classidx + ud_edge1;

            if (getCornersUDEdgesDepth3(idx1) == 3) {  // Entry not yet filled (3 means not reached)
              setCornersUDEdgesDepth3(idx1, (depth + 1) % 3);
              done += 1;

              // Symmetric position may have more than one representation
              unsigned short sym = c_sym[c1_classidx];
              if (sym != 1) {  // If sym is not just identity
                for (int k = 1; k < N_SYM_D4h; ++k) {
                  sym >>= 1;
                  if (sym % 2 == 1) {
                    int ud_edge2 = udEdgesConjTable_[(ud_edge1 << 4) + k];
                    // c1_classidx does not change
                    int idx2 = UD_EDGES_MAX * c1_classidx + ud_edge2;
                    if (getCornersUDEdgesDepth3(idx2) == 3) {
                      setCornersUDEdgesDepth3(idx2, (depth + 1) % 3);
                      done += 1;
                    }
                  }
                }
              }
            }
          }
        }
        ud_edge += 1;
        idx += 1;  // idx = N_UD_EDGES * c_classidx + ud_edge
      }
    }
    depth += 1;
  }

  std::ofstream file_out(filepath, std::ios::binary);
  if (file_out.is_open()) {
    file_out.write(reinterpret_cast<const char*>(cornersUDEdgesDepth3_.data()),
                   cornersUDEdgesDepth3_.size() * sizeof(uint32_t));
    file_out.close();
  }
}

void KociembaSolver::generateFlipSliceTwistDepth3Table() {
  const int total = FLIPSLICE_CLASS_MAX * TWIST_MAX;
  const std::string fname = "phase1_prun";
  const std::string filepath = getPrecomputedPath(fname);

  std::ifstream file_in(filepath, std::ios::binary);
  if (file_in.good()) {
    file_in.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      flipSliceTwistDepth3_.resize(total / 16 + 1);
      file.read(reinterpret_cast<char*>(flipSliceTwistDepth3_.data()), flipSliceTwistDepth3_.size() * sizeof(uint32_t));
      file.close();
    }
    return;
  }

  // Table doesn't exist, create it
  flipSliceTwistDepth3_.assign(total / 16 + 1, 0xffffffffu);

  // Create table with the symmetries of the flipslice classes
  std::vector<unsigned short> fs_sym(FLIPSLICE_CLASS_MAX, 0);
  EdgeCornerCube cc;
  KociembaSolver solver(cc);

  for (int i = 0; i < FLIPSLICE_CLASS_MAX; ++i) {
    uint32_t rep = flipSliceRep_[i];
    int slice = rep / FLIP_MAX;
    int flip = rep % FLIP_MAX;

    solver.ecCube_ = EdgeCornerCube();
    solver.coordCube.setSlice(slice);
    solver.coordCube.setFlip(flip);
    cc = solver.getCube();

    for (int s = 0; s < N_SYM_D4h; ++s) {
      EdgeCornerCube ss = symCubes_[s];
      ss.multiplyEdges(cc);
      ss.multiplyEdges(symCubes_[inverseSymIdx_[s]]);

      KociembaSolver temp_solver(ss);
      int new_slice = temp_solver.coordCube.getSlice();
      int new_flip = temp_solver.coordCube.getFlip();

      if (new_slice == slice && new_flip == flip) {
        fs_sym[i] |= 1 << s;
      }
    }
  }

  // Initialize the search
  int fs_classidx = 0;  // Value for solved phase 1
  int twist = 0;
  setFlipSliceTwistDepth3(TWIST_MAX * fs_classidx + twist, 0);
  int done = 1;
  int depth = 0;
  bool backsearch = false;

  while (done != total) {
    int depth3 = depth % 3;
    if (depth == 9) {
      // Switch to backwards search for depth >= 9
      backsearch = true;
    }

    int idx = 0;
    for (fs_classidx = 0; fs_classidx < FLIPSLICE_CLASS_MAX; ++fs_classidx) {
      twist = 0;
      while (twist < TWIST_MAX) {
        // Fast skip for empty regions
        if (!backsearch && idx % 16 == 0 && flipSliceTwistDepth3_[idx / 16] == 0xffffffffu && twist < TWIST_MAX - 16) {
          twist += 16;
          idx += 16;
          continue;
        }

        bool match;
        if (backsearch) {
          match = (getFlipSliceTwistDepth3(idx) == 3);
        } else {
          match = (getFlipSliceTwistDepth3(idx) == depth3);
        }

        if (match) {
          uint32_t flipslice = flipSliceRep_[fs_classidx];
          int flip = flipslice % FLIP_MAX;
          int slice = flipslice / FLIP_MAX;

          for (int m = 0; m < NUMBER_OF_MOVES; ++m) {
            int twist1 = twistMoveTable_[NUMBER_OF_MOVES * twist + m];
            int flip1 = flipMoveTable_[NUMBER_OF_MOVES * flip + m];
            int slice1 = sliceSortedMoveTable_[NUMBER_OF_MOVES * slice + m] / 24;  // N_PERM_4 = 24

            uint32_t flipslice1 = (slice1 * FLIP_MAX) + flip1;
            int fs1_classidx = flipSliceClassIdx_[flipslice1];
            int fs1_sym = flipSliceSym_[flipslice1];

            twist1 = twistConjTable_[(twist1 << 4) + fs1_sym];
            int idx1 = TWIST_MAX * fs1_classidx + twist1;

            if (!backsearch) {
              if (getFlipSliceTwistDepth3(idx1) == 3) {  // Entry not yet filled
                setFlipSliceTwistDepth3(idx1, (depth + 1) % 3);
                done += 1;

                // Symmetric position may have more than one representation
                unsigned short sym = fs_sym[fs1_classidx];
                if (sym != 1) {
                  for (int k = 1; k < N_SYM_D4h; ++k) {
                    if ((sym >> k) & 1) {
                      int twist2 = twistConjTable_[(twist1 << 4) + k];
                      int idx2 = TWIST_MAX * fs1_classidx + twist2;
                      if (getFlipSliceTwistDepth3(idx2) == 3) {
                        setFlipSliceTwistDepth3(idx2, (depth + 1) % 3);
                        done += 1;
                      }
                    }
                  }
                }
              }
            } else {  // Backwards search
              if (getFlipSliceTwistDepth3(idx1) == depth3) {
                setFlipSliceTwistDepth3(idx, (depth + 1) % 3);
                done += 1;
                break;
              }
            }
          }
        }
        twist += 1;
        idx += 1;  // idx = TWIST_MAX * fs_class + twist
      }
    }
    depth += 1;
  }

  std::ofstream file_out(filepath, std::ios::binary);
  if (file_out.is_open()) {
    file_out.write(reinterpret_cast<const char*>(flipSliceTwistDepth3_.data()),
                   flipSliceTwistDepth3_.size() * sizeof(uint32_t));
    file_out.close();
  }
}

void KociembaSolver::generateCornSliceDepthTable() {
  const std::string fname = "phase2_cornsliceprun";
  const std::string filepath = getPrecomputedPath(fname);

  std::cout << "Starting generation of cornslice_depth pruning table..." << std::endl;

  std::ifstream file_in(filepath, std::ios::binary);
  if (file_in.good()) {
    std::cout << "Found existing cornslice_depth table, loading..." << std::endl;
    file_in.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      cornSliceDepth_.resize(CORNERS_MAX * 24);  // N_PERM_4 = 24
      file.read(reinterpret_cast<char*>(cornSliceDepth_.data()), cornSliceDepth_.size() * sizeof(unsigned char));
      file.close();
      std::cout << "Loaded cornslice_depth table successfully." << std::endl;
    }
    return;
  }

  std::cout << "Creating new cornslice_depth table..." << std::endl;
  std::cout << "Total size: " << CORNERS_MAX * 24 << " entries" << std::endl;

  // Initialize the cornSliceDepth table with -1 values (indicating "not yet reached")
  cornSliceDepth_.assign(CORNERS_MAX * 24, 0xFF);  // Using 0xFF as -1

  // Set the solved state depth to 0
  cornSliceDepth_[0] = 0;  // corners = 0, slice = 0

  int depth = 0;
  int done = 1;
  int last_done = 0;
  int progress_threshold = (CORNERS_MAX * 24) / 100;  // Track every 1%

  std::cout << "Starting BFS: depth=" << depth << ", done=1/" << CORNERS_MAX * 24 << std::endl;

  // BFS to generate the pruning table
  while (done < CORNERS_MAX * 24) {
    int new_positions = 0;

    std::cout << "Processing depth " << depth << ", positions found so far: " << done << " ("
              << (done * 100.0 / (CORNERS_MAX * 24)) << "%)" << std::endl;

    for (int corners = 0; corners < CORNERS_MAX; ++corners) {
      if (corners % 10000 == 0) {
        std::cout << "  Processing corners: " << corners << "/" << CORNERS_MAX << "\r" << std::flush;
      }

      for (int slice = 0; slice < 24; ++slice) {  // N_PERM_4 = 24
        if (cornSliceDepth_[24 * corners + slice] == depth) {
          // Phase 2 moves: U1, U2, U3, R2, F2, D1, D2, D3, L2, B2
          for (int m : {0, 1, 2,   /*U1,U2,U3*/
                        4,         /*R2*/
                        7,         /*F2*/
                        9, 10, 11, /*D1,D2,D3*/
                        13,        /*L2*/
                        16 /*B2*/}) {
            int corners1 = cornersMoveTable_[NUMBER_OF_MOVES * corners + m];
            int slice1 = sliceSortedMoveTable_[NUMBER_OF_MOVES * slice + m];
            int idx1 = 24 * corners1 + slice1;

            if (cornSliceDepth_[idx1] == 0xFF) {  // Entry not yet filled
              cornSliceDepth_[idx1] = depth + 1;
              done += 1;
              new_positions += 1;

              // Show progress updates periodically
              if (done - last_done >= progress_threshold) {
                std::cout << "  Progress: " << done << "/" << CORNERS_MAX * 24 << " ("
                          << (done * 100.0 / (CORNERS_MAX * 24)) << "%)" << std::endl;
                last_done = done;
              }
            }
          }
        }
      }
    }

    depth += 1;
    std::cout << "Completed depth " << (depth - 1) << ", found " << new_positions << " new positions, total: " << done
              << "/" << CORNERS_MAX * 24 << " (" << (done * 100.0 / (CORNERS_MAX * 24)) << "%)" << std::endl;

    // If we didn't find any new positions at this depth, we might be stuck
    if (new_positions == 0) {
      std::cout << "WARNING: No new positions found at depth " << (depth - 1) << ". Table might be incomplete."
                << std::endl;
      break;
    }
  }

  std::cout << "Completed cornslice_depth table generation. Writing to file..." << std::endl;

  std::ofstream file_out(filepath, std::ios::binary);
  if (file_out.is_open()) {
    file_out.write(reinterpret_cast<const char*>(cornSliceDepth_.data()),
                   cornSliceDepth_.size() * sizeof(unsigned char));
    file_out.close();
    std::cout << "Successfully wrote cornslice_depth table to " << filepath << std::endl;
  } else {
    std::cerr << "ERROR: Failed to write cornslice_depth table to " << filepath << std::endl;
  }
}

void KociembaSolver::generatePhase2EdgeMergeTable() {
  const std::string fname = "phase2_edgemerge";
  const std::string filepath = getPrecomputedPath(fname);

  std::cout << "Starting generation of phase2_edgemerge table..." << std::endl;

  std::ifstream file_in(filepath, std::ios::binary);
  if (file_in.good()) {
    std::cout << "Found existing phase2_edgemerge table, loading..." << std::endl;
    file_in.close();
    std::ifstream file(filepath, std::ios::binary);
    if (file.is_open()) {
      phase2EdgeMergeTable_.resize(SLICE_SORTED_MAX * 24);  // N_PERM_4 = 24
      file.read(reinterpret_cast<char*>(phase2EdgeMergeTable_.data()),
                phase2EdgeMergeTable_.size() * sizeof(unsigned short));
      file.close();
      std::cout << "Successfully loaded phase2_edgemerge table" << std::endl;
    }
    return;
  }

  std::cout << "Creating new phase2_edgemerge table..." << std::endl;

  // The table maps (u_edges, d_edges) -> ud_edges
  const int N_U_EDGES_PHASE2 = SLICE_SORTED_MAX;
  const int N_PERM_4 = 24;

  // Initialize the table
  phase2EdgeMergeTable_.resize(N_U_EDGES_PHASE2 * N_PERM_4, 0);

  int cnt = 0;
  int progress_marker = 0;

  for (int i = 0; i < N_U_EDGES_PHASE2; ++i) {
    if (i % 100 == 0) {
      std::cout << "Processing u_edges: " << i << "/" << N_U_EDGES_PHASE2 << " (" << (i * 100.0 / N_U_EDGES_PHASE2)
                << "%)\r" << std::flush;
    }

    // Set up cube with these u_edges
    EdgeCornerCube c_u;
    KociembaSolver solver_u(c_u);
    solver_u.coordCube.setUEdges(i);
    c_u = solver_u.getCube();

    for (int j = 0; j < SLICE_MAX; ++j) {  // Iterate through d_edges combinations (C(8,4))
      // Set up a cube with this d_edges configuration at position 0
      EdgeCornerCube c_d;
      KociembaSolver solver_d(c_d);
      solver_d.coordCube.setDEdges(j * N_PERM_4);  // Use permutation 0
      c_d = solver_d.getCube();

      // Check if we have a valid combined state (no edge collisions)
      bool invalid = false;
      EdgeCornerCube c_ud;

      // Check upper face edges
      for (int e = 0; e <= 7; ++e) {  // UR through DB (all U/D layer edges)
        EdgeID pos_u = c_u.getEdge(e).position;
        EdgeID pos_d = c_d.getEdge(e).position;

        // Check for collision (same edge in both configurations)
        if ((pos_u <= EdgeID::UB && pos_d <= EdgeID::UB) ||  // Both in U layer
            (pos_u >= EdgeID::DR && pos_u <= EdgeID::DB && pos_d >= EdgeID::DR &&
             pos_d <= EdgeID::DB)) {  // Both in D layer
          invalid = true;
          break;
        }
      }

      if (!invalid) {
        // Process all permutations of the d_edges
        for (int k = 0; k < N_PERM_4; ++k) {
          // Set up cube with this specific d_edges permutation
          EdgeCornerCube c_d_perm;
          KociembaSolver solver_d_perm(c_d_perm);
          solver_d_perm.coordCube.setDEdges(j * N_PERM_4 + k);
          c_d_perm = solver_d_perm.getCube();

          // Combine the two configurations
          EdgeCornerCube c_combined;
          // Copy edge positions from u_edges and d_edges to create combined state
          for (int e = 0; e <= 7; ++e) {  // UR through DB
            EdgeID pos_u = c_u.getEdge(e).position;
            EdgeID pos_d = c_d_perm.getEdge(e).position;

            if (pos_u <= EdgeID::UB) {  // U layer edge from u_edges
              c_combined.setEdge(e, {pos_u, c_u.getEdge(e).orientation});
            } else if (pos_d >= EdgeID::DR && pos_d <= EdgeID::DB) {  // D layer edge from d_edges
              c_combined.setEdge(e, {pos_d, c_d_perm.getEdge(e).orientation});
            }
          }

          // Get the ud_edges coordinate for this combined state
          KociembaSolver solver_combined(c_combined);
          int ud_edges = solver_combined.coordCube.getUDEdges();

          // Store in the table
          phase2EdgeMergeTable_[N_PERM_4 * i + k] = ud_edges;

          cnt++;
          if (cnt % 2000 == 0) {
            progress_marker++;
            std::cout << "." << std::flush;
            if (progress_marker % 50 == 0) {
              std::cout << " " << cnt << " entries\n" << std::flush;
            }
          }
        }
      }
    }
  }

  std::cout << "\nCompleted phase2_edgemerge table generation with " << cnt << " entries. Writing to file..."
            << std::endl;

  std::ofstream file_out(filepath, std::ios::binary);
  if (file_out.is_open()) {
    file_out.write(reinterpret_cast<const char*>(phase2EdgeMergeTable_.data()),
                   phase2EdgeMergeTable_.size() * sizeof(unsigned short));
    file_out.close();
    std::cout << "Successfully wrote phase2_edgemerge table to " << filepath << std::endl;
  } else {
    std::cerr << "ERROR: Failed to write phase2_edgemerge table to " << filepath << std::endl;
  }
}

void KociembaSolver::generateDistanceArray() {
  std::cout << "Generating distance array..." << std::endl;

  // Initialize the distance array with 60 elements
  distance_.resize(60, 0);

  // Implement the same logic as the Python code
  for (int i = 0; i < 20; ++i) {
    for (int j = 0; j < 3; ++j) {
      distance_[3 * i + j] = (i / 3) * 3 + j;

      if (i % 3 == 2 && j == 0) {
        distance_[3 * i + j] += 3;
      } else if (i % 3 == 0 && j == 2) {
        distance_[3 * i + j] -= 3;
      }
    }
  }

  std::cout << "Distance array generation complete." << std::endl;
}

}  // namespace Kociemba
