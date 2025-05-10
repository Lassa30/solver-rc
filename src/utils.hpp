#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

std::string readShader(const std::string &shaderFile) {
  std::string shaderCode;
  std::ifstream shaderStream(shaderFile, std::ios::in);
  if (shaderStream.is_open()) {
    std::stringstream sstr;
    sstr << shaderStream.rdbuf();
    shaderCode = sstr.str();
    shaderStream.close();
  } else {
    return "No code!";
  }

  return shaderCode;
}