#pragma once

#include "IntInstr.hpp"
#include "IntInstrBlock.hpp"
#include "Condition.hpp"

class IntInstrWhile : IntInstrAbstr
{
  IntInstrBlock subBlock, revSubBlock;
  
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
};
