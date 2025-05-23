#include <cube/cube.hpp>
#include <iostream>
#include <kociemba-solver/solver.hpp>

namespace Kociemba {

// Define all static members of KociembaSolver
std::array<EdgeCornerCube, static_cast<int>(BasicSymmetry::COUNT)> KociembaSolver::basicSymCubes_;
std::array<EdgeCornerCube, NUMBER_OF_SYMMETRIES> KociembaSolver::symCubes_;
std::array<int, NUMBER_OF_SYMMETRIES> KociembaSolver::inverseSymIdx_;
std::array<int, NUMBER_OF_MOVES * NUMBER_OF_SYMMETRIES> KociembaSolver::conjMoves_;

std::vector<unsigned short> KociembaSolver::twistConjTable_;
std::vector<unsigned short> KociembaSolver::flipSliceClassIdx_;
std::vector<unsigned char> KociembaSolver::flipSliceSym_;
std::vector<uint32_t> KociembaSolver::flipSliceRep_;

std::vector<unsigned short> KociembaSolver::cornerClassIdx_;
std::vector<unsigned char> KociembaSolver::cornerSym_;
std::vector<unsigned short> KociembaSolver::cornerRep_;

std::vector<unsigned short> KociembaSolver::twistMoveTable_;
std::vector<unsigned short> KociembaSolver::flipMoveTable_;
std::vector<unsigned short> KociembaSolver::sliceSortedMoveTable_;
std::vector<unsigned short> KociembaSolver::uEdgesMoveTable_;
std::vector<unsigned short> KociembaSolver::dEdgesMoveTable_;
std::vector<unsigned short> KociembaSolver::udEdgesMoveTable_;
std::vector<unsigned short> KociembaSolver::cornersMoveTable_;

std::vector<uint32_t> KociembaSolver::flipSliceTwistDepth3_;
std::vector<uint32_t> KociembaSolver::cornersUDEdgesDepth3_;
std::vector<unsigned char> KociembaSolver::cornSliceDepth_;
std::vector<char> KociembaSolver::distance_;
std::vector<unsigned short> KociembaSolver::phase2EdgeMergeTable_;

std::vector<unsigned short> KociembaSolver::udEdgesConjTable_;

// Initialize static booleans
bool KociembaSolver::isBasicSymCubesGenerated_ = [] {
  std::cout << "Generating basic symmetry cubes..." << std::endl;
  KociembaSolver::generateBasicSymCubes();
  std::cout << "Finished generating basic symmetry cubes." << std::endl;
  return true;
}();

bool KociembaSolver::isSymCubesGenerated_ = [] {
  std::cout << "Generating symmetry cubes..." << std::endl;
  KociembaSolver::generateSymCubes();
  std::cout << "Finished generating symmetry cubes." << std::endl;
  return true;
}();

bool KociembaSolver::isInverseSymIdxGenerated_ = [] {
  std::cout << "Generating inverse symmetry indices..." << std::endl;
  KociembaSolver::generateInverseSymIdx();
  std::cout << "Finished generating inverse symmetry indices." << std::endl;
  return true;
}();

bool KociembaSolver::isConjMovesGenerated_ = [] {
  std::cout << "Generating conjugate moves..." << std::endl;
  KociembaSolver::generateConjMove();
  std::cout << "Finished generating conjugate moves." << std::endl;
  return true;
}();

bool KociembaSolver::isTwistConjTableGenerated_ = [] {
  std::cout << "Generating twist conjugation table..." << std::endl;
  KociembaSolver::generateTwistConjTable();
  std::cout << "Finished generating twist conjugation table." << std::endl;
  return true;
}();

bool KociembaSolver::isFlipSliceTablesGenerated_ = [] {
  std::cout << "Generating flip-slice tables..." << std::endl;
  KociembaSolver::generateFlipSliceTables();
  std::cout << "Finished generating flip-slice tables." << std::endl;
  return true;
}();

bool KociembaSolver::isCornerTablesGenerated_ = [] {
  std::cout << "Generating corner tables..." << std::endl;
  KociembaSolver::generateCornerTables();
  std::cout << "Finished generating corner tables." << std::endl;
  return true;
}();

bool KociembaSolver::isTwistMoveTableGenerated_ = [] {
  std::cout << "Generating twist move table..." << std::endl;
  KociembaSolver::generateTwistMoveTable();
  std::cout << "Finished generating twist move table." << std::endl;
  return true;
}();

bool KociembaSolver::isFlipMoveTableGenerated_ = [] {
  std::cout << "Generating flip move table..." << std::endl;
  KociembaSolver::generateFlipMoveTable();
  std::cout << "Finished generating flip move table." << std::endl;
  return true;
}();

bool KociembaSolver::isSliceSortedMoveTableGenerated_ = [] {
  std::cout << "Generating slice sorted move table..." << std::endl;
  KociembaSolver::generateSliceSortedMoveTable();
  std::cout << "Finished generating slice sorted move table." << std::endl;
  return true;
}();

bool KociembaSolver::isUEdgesMoveTableGenerated_ = [] {
  std::cout << "Generating U edges move table..." << std::endl;
  KociembaSolver::generateUEdgesMoveTable();
  std::cout << "Finished generating U edges move table." << std::endl;
  return true;
}();

bool KociembaSolver::isDEdgesMoveTableGenerated_ = [] {
  std::cout << "Generating D edges move table..." << std::endl;
  KociembaSolver::generateDEdgesMoveTable();
  std::cout << "Finished generating D edges move table." << std::endl;
  return true;
}();

bool KociembaSolver::isUDEdgesMoveTableGenerated_ = [] {
  std::cout << "Generating UD edges move table..." << std::endl;
  KociembaSolver::generateUDEdgesMoveTable();
  std::cout << "Finished generating UD edges move table." << std::endl;
  return true;
}();

bool KociembaSolver::isCornersMoveTableGenerated_ = [] {
  std::cout << "Generating corners move table..." << std::endl;
  KociembaSolver::generateCornersMoveTable();
  std::cout << "Finished generating corners move table." << std::endl;
  return true;
}();

// Initialize UDEdges conjugation table before cornersUDEdgesDepth3 to address the dependency
bool KociembaSolver::isUDEdgesConjTableGenerated_ = [] {
  std::cout << "Generating UD edges conjugation table..." << std::endl;
  KociembaSolver::generateUDEdgesConjTable();
  std::cout << "Finished generating UD edges conjugation table." << std::endl;
  return true;
}();

bool KociembaSolver::isFlipSliceTwistDepth3TableGenerated_ = [] {
  KociembaSolver::generateFlipSliceTwistDepth3Table();
  return true;
}();

bool KociembaSolver::isCornersUDEdgesDepth3TableGenerated_ = [] {
  KociembaSolver::generateCornersUDEdgesDepth3Table();
  return true;
}();

bool KociembaSolver::isCornSliceDepthTableGenerated_ = [] {
  KociembaSolver::generateCornSliceDepthTable();
  return true;
}();

bool KociembaSolver::isDistanceArrayGenerated_ = [] {
  KociembaSolver::generateDistanceArray();
  return true;
}();

bool KociembaSolver::isPhase2EdgeMergeTableGenerated_ = [] {  // Added initialization for new table
  std::cout << "Generating phase2_edgemerge table..." << std::endl;
  KociembaSolver::generatePhase2EdgeMergeTable();
  std::cout << "Finished generating phase2_edgemerge table." << std::endl;
  return true;
}();

}  // namespace Kociemba
