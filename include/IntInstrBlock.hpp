#pragma once

#include <vector>
#include <string>
#include <memory>

#include "IntInstr.hpp"
#include "InstructionRegistry.hpp"
#include "VariableRegistry.hpp"

class IntInstrBlock : IntInstrAbstr
{
  std::vector<std::shared_ptr<IntInstrAbstr>> instructions;

public:

  void addInstr(IntInstrAbstr* instr);

  std::vector<std::shared_ptr<IntInstrAbstr>> getInstructions();

  void append(IntInstrBlock block);

  bool empty();

  void dbgPrint() override;

  InstructionRegistry translate(VariableRegistry& variables) override;

  void optimize() override;

  void propagateConstants(std::unordered_map<std::string, std::string>& constants) override;

  bool modifiesVariable(std::string name) override;

  // void useOfConstants(std::unordered_map<std::string, lint> usage);
  // void replaceConstWithVar(std::string constant, std::string variable);
};
