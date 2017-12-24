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

  void dbgPrint() override;

  InstructionRegistry translate(VariableRegistry& variables) override;

  void optimize() override;
};
