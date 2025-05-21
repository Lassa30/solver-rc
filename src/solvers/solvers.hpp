#pragma once

#include <cube/cube.hpp>

#include <array>
#include <bitset>
#include <cstdint>  // Required for uint32_t
#include <string>
#include <vector>

using namespace Cube;

namespace Kociemba {

// Enum for basic symmetries (assuming it's defined, e.g. from previous context)
enum class BasicSymmetry { ROT_URF3, ROT_F2, ROT_U4, MIRR_LR2, COUNT };
// Constant for number of symmetries (assuming it's defined)
static constexpr int NUMBER_OF_SYMMETRIES = 48;
static constexpr int N_SYM_D4h = 16;               // D4h symmetry subgroup size
static constexpr int TWIST_MAX = 2187;             // 3^7 corner orientations
static constexpr int FLIP_MAX = 2048;              // 2^11 edge orientations
static constexpr int SLICE_MAX = 495;              // C(12,4)
static constexpr int SLICE_SORTED_MAX = 11880;     // N_SLICE_SORTED from Python (12!/8!)
static constexpr int FLIPSLICE_CLASS_MAX = 64430;  // Number of equivalence classes for flip+slice
static constexpr int CORNERS_MAX = 40320;          // 8! corner permutations
static constexpr int CORNERS_CLASS_MAX = 2768;     // Number of equivalence classes for corner permutations
static constexpr int UD_EDGES_MAX = 40320;         // N_UD_EDGES from Python (8!)
static constexpr int UD_EDGES_CONJ_MAX = UD_EDGES_MAX * N_SYM_D4h;  // For udEdgesConjTable_
static constexpr unsigned short INVALID_SHORT = 0xFFFF;

/* Kociemba's algorithm: two-phase algorithm */
class KociembaSolver {
public:
  explicit KociembaSolver(const EdgeCornerCube&);

  inline EdgeCornerCube getCube() const { return ecCube_; }
  std::string toString() const;

  /*symmetry related things for speeding up the process*/
  EdgeCornerCube inverseCube() const;
  std::bitset<96> symmetriesCube() const;

  /*symmetry checkers*/
  bool hasRotationalSymmetry(const std::bitset<96>&) const;
  bool hasAntiSymmetry(const std::bitset<96>&) const;

  /*main solve function: idk the interface yet*/
  std::string solve();

  class CoordinateCube {
  public:
    explicit CoordinateCube(KociembaSolver* solver);

    /*phase 1 getters*/
    int getTwist() const;
    int getFlip() const;
    int getSlice() const;
    int getSliceSorted() const;

    /*setters for phase 1*/
    void setTwist(int twist);
    void setFlip(int flip);
    void setSlice(int idx);
    void setSliceSorted(int idx);

    /*phase 2 getters*/
    int getUEdges() const;
    int getDEdges() const;
    int getCorners() const;
    int getUDEdges() const;

    /*setters for phase 2*/
    void setUEdges(int idx);
    void setDEdges(int idx);
    void setCorners(int idx);
    void setUDEdges(int idx);

    int getCornersUDEdgesDepth3(int ix);

    struct phaseOne {
      phaseOne() : twist{SOLVED}, flip{SOLVED}, slice_sorted{SOLVED} {}
      phaseOne(int c1, int c2, int c3) : twist{c1}, flip{c2}, slice_sorted{c3} {}
      int twist;
      int flip;
      int slice_sorted;
    };

    struct phaseTwo {
      phaseTwo() : u_edges{1656}, d_edges{SOLVED}, corners{SOLVED}, ud_edges{SOLVED} {};
      phaseTwo(int c1, int c2, int c3, int c4) : u_edges{c1}, d_edges{c2}, corners{c3}, ud_edges{c4} {};
      int u_edges;
      int d_edges;
      int corners;
      int ud_edges;
    };

    static constexpr int SOLVED = 0;
    phaseOne coordsPhaseOne;
    phaseTwo coordsPhaseTwo;

  private:
    KociembaSolver* solver_;

    /*helpers for coordinate computation*/
    static int c_nk(int n, int k);

    template <typename Container>
    static void rotate_right(Container& arr, int left, int right);

    template <typename Container>
    static void rotate_left(Container& arr, int left, int right);
  };
  CoordinateCube coordCube;

private:
  EdgeCornerCube ecCube_;

  /*members for cube symmetries generation*/
  static bool isBasicSymCubesGenerated_;
  static std::array<EdgeCornerCube, static_cast<int>(BasicSymmetry::COUNT)> basicSymCubes_;
  static void generateBasicSymCubes();

  static bool isSymCubesGenerated_;
  static std::array<EdgeCornerCube, NUMBER_OF_SYMMETRIES> symCubes_;
  static void generateSymCubes();

  static bool isInverseSymIdxGenerated_;
  static std::array<int, NUMBER_OF_SYMMETRIES> inverseSymIdx_;
  static void generateInverseSymIdx();

  static bool isConjMovesGenerated_;
  static std::array<int, NUMBER_OF_MOVES * NUMBER_OF_SYMMETRIES> conjMoves_;
  static void generateConjMove();

  /*static members for symmetry conjugation tables*/
  static bool isTwistConjTableGenerated_;
  static std::vector<unsigned short> twistConjTable_;
  static void generateTwistConjTable();

  /*static members for flip-slice symmetry tables*/
  static bool isFlipSliceTablesGenerated_;
  static std::vector<unsigned short> flipSliceClassIdx_;  // idx -> classidx
  static std::vector<unsigned char> flipSliceSym_;        // idx -> symmetry
  static std::vector<uint32_t> flipSliceRep_;             // classidx -> idx of representant
  static void generateFlipSliceTables();

  /*static members for corner permutation symmetry tables*/
  static bool isCornerTablesGenerated_;
  static std::vector<unsigned short> cornerClassIdx_;  // idx -> classidx
  static std::vector<unsigned char> cornerSym_;        // idx -> symmetry
  static std::vector<unsigned short> cornerRep_;       // classidx -> idx of representant
  static void generateCornerTables();

  /*static members for move tables*/
  static bool isTwistMoveTableGenerated_;
  static std::vector<unsigned short> twistMoveTable_;
  static void generateTwistMoveTable();

  static bool isFlipMoveTableGenerated_;
  static std::vector<unsigned short> flipMoveTable_;
  static void generateFlipMoveTable();

  static bool isSliceSortedMoveTableGenerated_;
  static std::vector<unsigned short> sliceSortedMoveTable_;
  static void generateSliceSortedMoveTable();

  static bool isUEdgesMoveTableGenerated_;
  static std::vector<unsigned short> uEdgesMoveTable_;
  static void generateUEdgesMoveTable();

  static bool isDEdgesMoveTableGenerated_;
  static std::vector<unsigned short> dEdgesMoveTable_;
  static void generateDEdgesMoveTable();

  static bool isUDEdgesMoveTableGenerated_;
  static std::vector<unsigned short> udEdgesMoveTable_;
  static void generateUDEdgesMoveTable();

  static bool isCornersMoveTableGenerated_;
  static std::vector<unsigned short> cornersMoveTable_;
  static void generateCornersMoveTable();

  /*static pruning tables*/
  static std::vector<uint32_t> flipSliceTwistDepth3_;
  static std::vector<uint32_t> cornersUDEdgesDepth3_;
  static std::vector<unsigned char> cornSliceDepth_;
  static std::vector<char> distance_;
  static std::vector<unsigned short> phase2EdgeMergeTable_;  // Added for phase2_edgemerge table

  static bool isFlipSliceTwistDepth3TableGenerated_;
  static bool isCornersUDEdgesDepth3TableGenerated_;
  static bool isCornSliceDepthTableGenerated_;
  static bool isDistanceArrayGenerated_;
  static bool isPhase2EdgeMergeTableGenerated_;  // Added flag for phase2_edgemerge table

  /*static pruning table generation methods*/
  static void generateCornersUDEdgesDepth3Table();
  static void generateFlipSliceTwistDepth3Table();
  static void generateCornSliceDepthTable();
  static void generateDistanceArray();
  static void generatePhase2EdgeMergeTable();  // Added generation method declaration

  static bool isUDEdgesConjTableGenerated_;
  static std::vector<unsigned short> udEdgesConjTable_;
  static void generateUDEdgesConjTable();

  static unsigned char getFlipSliceTwistDepth3(int ix);
  static void setFlipSliceTwistDepth3(int ix, unsigned char value);
  static unsigned char getCornersUDEdgesDepth3(int ix);
  static void setCornersUDEdgesDepth3(int ix, unsigned char value);

  static std::string getPrecomputedPath(const std::string& filename);
};

}  // namespace Kociemba