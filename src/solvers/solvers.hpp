#pragma once

#include <cube/cube.hpp>

#include <array>
#include <bitset>
#include <memory>  // Required for std::unique_ptr if used, but we'll use KociembaSolver*
#include <string>

using namespace Cube;

namespace Kociemba {

// Enum for basic symmetries (assuming it's defined, e.g. from previous context)
enum class BasicSymmetry { ROT_URF3, ROT_F2, ROT_U4, MIRR_LR2, COUNT };
// Constant for number of symmetries (assuming it's defined)
static constexpr int NUMBER_OF_SYMMETRIES = 48;

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
  static std::array<EdgeCornerCube, static_cast<int>(BasicSymmetry::COUNT)> basicSymCubes_;
  static std::array<EdgeCornerCube, NUMBER_OF_SYMMETRIES> symCubes_;
  static std::array<int, NUMBER_OF_SYMMETRIES> inverseSymIdx_;

  static bool isBasicSymCubesGenerated_;
  static bool isSymCubesGenerated_;
  static bool isInverseSymIdxGenerated_;

  static void generateBasicSymCubes();
  static void generateSymCubes();
  static void generateInverseSymIdx();

  /*static members for pruning tables generation*/

  /*static members for move tables generation*/
};

}  // namespace Kociemba