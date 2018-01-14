#pragma once

#include <string>
#include <set>
#include <unordered_map>

#include "general.hpp"

class Variables
{
public:
  std::set<std::string> variables;
  std::unordered_map<std::string,lint> arrays;



  void addVariable(std::string name);

  void addArrayVariable(std::string name, lint size);
};
