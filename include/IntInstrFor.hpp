#pragma once

#include "IntInstr.hpp"
#include "IntInstrBlock.hpp"

class IntInstrFor : IntInstrAbstr
{
  IntInstrBlock subBlock;

  std::string iterator;
  std::string val1, val2;
  bool downTo;

  IntInstrBlock block;

public:
  IntInstrFor(std::string iterator, std::string val1, std::string val2, bool downTo, IntInstrBlock block, IntInstrBlock valInitBlock);

  void dbgPrint() override;

  InstructionRegistry translate(VariableRegistry& variables) override;

  void optimize() override;

  void propagateConstants(std::unordered_map<std::string, std::string>& constants) override;

  bool modifiesVariable(std::string name) override;

  // void useOfConstants(std::unordered_map<std::string, lint> usage);
  // void replaceConstWithVar(std::string constant, std::string variable);
};
