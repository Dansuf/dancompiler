#pragma once

#include "IntInstr.hpp"
#include "IntInstrBlock.hpp"
#include "Condition.hpp"


class IntInstrIf : IntInstrAbstr
{
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
  IntInstrIf(std::string val1, std::string val2, Comparator comp, IntInstrBlock block): val1(val1), val2(val2), comp(comp), block(block) {}

  void dbgPrint() override;

  InstructionRegistry translate(VariableRegistry& variables) override;

  void optimize() override;
};
