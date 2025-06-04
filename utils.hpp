#pragma once
#include <iostream>
#include <cpr/cpr.h>

void updCubeString(const char& move) {
  cpr::Response RR = cpr::Get(cpr::Url{"http://localhost:8081/move"},
                      cpr::Parameters{{"move", std::string(1, move)}});
}

std::string solveCube() {
  cpr::Response RR = cpr::Get(cpr::Url{"http://localhost:8081/solve"});
  return RR.text;
}