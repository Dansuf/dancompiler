#pragma once

#include "IntInstr.hpp"
#include "IntInstrBlock.hpp"
#include "Condition.hpp"

class IntInstrWhile : IntInstrAbstr
{
  IntInstrBlock subBlock, revSubBlock, valInitBlock;

  std::string val1;
  std::string val2;
  Comparator comp;

  IntInstrBlock block;

  InstructionRegistry translateEq(VariableRegistry& variables);
  InstructionRegistry translateNeq(VariableRegistry& variables);
  InstructionRegistry translateLt(VariableRegistry& variables);
  InstructionRegistry translateGt(VariableRegistry& variables);
  InstructionRegistry translateLte(VariableRegistry& variables);
  InstructionRegistry translateGte(VariableRegistry& variables);

public:
  IntInstrWhile(std::string val1, std::string val2, Comparator comp, IntInstrBlock block, IntInstrBlock valInitBlock);

  void dbgPrint() override;

  InstructionRegistry translate(VariableRegistry& variables) override;

  void optimize() override;

  void propagateConstants(std::unordered_map<std::string, std::string>& constants) override;

  bool modifiesVariable(std::string name) override;

  // void useOfConstants(std::unordered_map<std::string, lint> usage);
  // void replaceConstWithVar(std::string constant, std::string variable);
};
