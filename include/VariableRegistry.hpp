#pragma once

#include <unordered_map>
#include <set>

#include "general.hpp"

#define INT_TEMPS 5
#define ASSEMBLER_TEMPS 5

class VariableRegistry
{
  lint lastFreeIndex = INT_TEMPS + ASSEMBLER_TEMPS;
  std::unordered_map<std::string,lint> indexes;
  //TODO store info if initialized

  std::unordered_map<std::string,lint> arrays;

  std::set<std::string> iterators;
  std::set<std::string> activeIterators;

  lint lastIntTempAddr = 0;
  lint lastAssemblerTempAddr = INT_TEMPS;

  lint lastLabelId = 0;

  lint lastFreeCounter = 0;

public:
  VariableRegistry();

  lint addVariable(std::string name);
  lint addArrayVariable(std::string name, lint size);

  lint getIndex(std::string name);

  std::string getIntTemp();
  void freeIntTemps();

  std::string getAssemblerTemp();
  void freeAssemblerTemps();

  lint setIterator(std::string name);
  void unsetIterator(std::string name);

  lint getForCounter();

  lint newLabel();

  void assertLoadableVariable(std::string name);
  void assertStorableVariable(std::string name);
  void assertArrayVariable(std::string name, std::string index);

  static bool isPointer(std::string variable);
  static bool isConst(std::string variable);

  static std::string stripPointer(std::string variable);
  static lint getConstVal(std::string value);

  static std::string toConst(lint val);

  std::unordered_map<std::string,lint> getArrays();
};
